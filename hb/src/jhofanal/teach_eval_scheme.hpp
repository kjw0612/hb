#ifndef teach_eval_scheme_h__
#define teach_eval_scheme_h__

#include "settings.hpp"
#include "csvparser.hpp"
#include "csvparserfast.hpp"
#include "optimization_method.hpp"


//is_sell	is_buy	is_bidinsertion	is_askinsertion	is_bidcancelled	is_askcancelled	dir_bidmoved	dir_askmoved
inline vs sbnames(){
	vs ret; ret.push_back("is_sell"); ret.push_back("is_buy");
	return ret;
}
inline vs bamnames(){
	vs ret; ret.push_back("dir_bidmoved"); ret.push_back("dir_askmoved");
	return ret;
}
inline vs bainames(){
	vs ret; ret.push_back("is_bidinsertion"); ret.push_back("is_askinsertion");
	return ret;
}
inline vs bacnames(){
	vs ret; ret.push_back("is_bidcancelled"); ret.push_back("is_askcancelled");
	return ret;
}
inline vs qtynames(){
	vs ret; ret.push_back("bidQty1"); ret.push_back("askQty1");
	return ret;
}
inline vs qtynorms(){
	vs ret; ret.push_back("a+bQty1"); ret.push_back("a/(a+b)Qty1");
	return ret;
}
template<class vt>
vt& concat(vt& v1, vt& v2){
	v1.insert( v1.end(), v2.begin(), v2.end() );
	return v1;
}

class LearningSystem{
public:
	LearningSystem() : isOptimized_(false) {}
	virtual vs xnames() = 0;
	virtual vs ynames() {
		vs ret; ret.push_back("dir_bidmoved"); ret.push_back("dir_askmoved");
		return ret;
	}
	virtual void add(const vd& x, const vd& y) = 0;
	virtual void adds(const vvd& xs, const vvd& ys) {
		assert(xs.size()==ys.size());
		for (int i=0;i<(int)xs.size();++i) add(xs[i],ys[i]);
	}
	virtual void lazyOptimize() {}
	void optimize() {
		if (!isOptimized_){
			isOptimized_ = true;
			lazyOptimize();
		}
	}
	virtual double error(const vd& x, const vd& y) = 0;
	virtual pair<double, int> errors(const vvd& xs, const vvd& ys) {
		double ret = 0;
		for (int i=0;i<(int)xs.size();++i) ret += error(xs[i], ys[i]);
		return make_pair(ret, (int)xs.size());
	}
	bool isOptimized_;
};

class Mto1System : public LearningSystem{
public:
	Mto1System (const vs& xnames_, int nn = 1) : xnames_(xnames_), nn(nn) {}
	template<class T>
	static int hashy(const vector<T>& y){
		//assert(y.size()==1); // only damn single value suitable as input.
		T ret = 0;
		for (int i=0;i<(int)y.size();++i) ret += y[i];
		return (ret > 0);
	}
	vs xnames(){ return xnames_; }
	vvd remakeX(const vvd& xs){
		vvd ret;
		for (int i=0;i+nn<(int)xs.size();i+=nn){
			vd vr;
			for (int j=i;j<i+nn;++j){
				vr.insert(vr.end(),xs[j].begin(),xs[j].end());
			}
			ret.push_back(vr);
		}
		return ret;
	}
	vvd remakeY(const vvd& ys){
		vvd ret;
		for (int i=0;i+nn<(int)ys.size();i+=nn){
			ret.push_back(ys[i+nn-1]);
		}
		return ret;
	}
	virtual void adds(const vvd& xs, const vvd& ys){
		if (nn==1) LearningSystem::adds(xs,ys);
		else LearningSystem::adds(remakeX(xs),remakeY(ys));
	}
	pair<double, int> errors(const vvd& xs, const vvd& ys){
		LearningSystem::optimize();
		if (nn==1) return LearningSystem::errors(xs,ys);
		else return LearningSystem::errors(remakeX(xs),remakeY(ys));
	}
	int nn; vs xnames_;
};

class ParamSystem : public Mto1System, public CostFunction{
public:
	ParamSystem (const vs& xnames_, int nn = 1) : Mto1System(xnames_, nn) {}

	virtual double eval(const vd& x, const vd& param) const = 0;
	void add(const vd& x, const vd& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	virtual void adds(const vvd& xs_, const vvd& ys_){
		vvd xxs_ = remakeX(xs_), yys_ = remakeY(ys_);
		xs.insert(xs.end(),xxs_.begin(),xxs_.end());
		ys.insert(ys.end(),yys_.begin(),yys_.end());
	}
	inline double error(const vd &x, const vd& y){
		double y_real = (double)hashy(y);
		return fabs(y_real - eval(x, paramset));
	}
	virtual double costFunction(const vd& param) const{
		double cf = 0;
		for (int i=0;i<(int)xs.size();++i){
			cf += pow(eval(xs[i], param) - hashy(ys[i]),2);
		}
		return cf / xs.size();
	}
	vvd xs, ys;
	vd paramset;
};

#define TRIM_SPACE " \t\n\v"
namespace ospace {
	inline std::string trim(std::string& s,const std::string& drop = TRIM_SPACE){
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}
	inline std::string rtrim(std::string s,const std::string& drop = TRIM_SPACE){
		return s.erase(s.find_last_not_of(drop)+1);
	}
	inline std::string ltrim(std::string s,const std::string& drop = TRIM_SPACE){
		return s.erase(0,s.find_first_not_of(drop));
	}
}

int strcmpitr(const string& a, const string& b){
	string a_(a), b_(b);
	ospace::trim(a_), ospace::trim(b_);
	return _strcmpi(a_.c_str(), b_.c_str());
}

class TeachEvalScheme{
public:
	enum DataType{
		Training = 0,
		Test = 1,
		CrossValidation = 2,
	};
	void addSet(const string& filename, DataType datatype){
		dataset[datatype].push_back(filename);
	}
	static pair<vvd, vvd> getDataFast(const string& filename, const vs& xnames, const vs& ynames){
		int volidx = 0;
		vi xidxs(xnames.size(),-1), yidxs(ynames.size(),-1);
		vi aloneflag(xnames.size(),0);
		int allone = 0;
		vvd xs; vvd ys;
		xs.reserve(200000),ys.reserve(200000);
		CsvParserFast cp(filename);
		int m = cp.getline(1);
		for (int i=0;i<(int)xnames.size();++i){
			aloneflag[i] = !(!strcmpitr(xnames[i],"bidqty1") || !strcmpitr(xnames[i],"askqty1"));
			allone |= aloneflag[i];
		}
		if (!allone){
			for (int i=0;i<(int)xnames.size();++i)
				aloneflag[i] = 1;
		}
		for (int k=0;k<2;++k){
			const vs& names = (k==0) ? xnames : ynames;
			vi& idxs = (k==0) ? xidxs : yidxs;
			for (int j=0;j<(int)names.size();++j){
				for (int i=0;i<m;++i){
					if (!strcmpitr(names[j], cp.line[i]))
						idxs[j] = i;
					if (!strcmpitr("vol",cp.line[i]))
						volidx = i;
				}
				assert(idxs[j]>=0);
			}
		}
		while (m = cp.getline(0)){
			for (int k=0;k<2;++k){
				const vs& names = (k==0) ? xnames : ynames;
				vi& idxs = (k==0) ? xidxs : yidxs;
				vvd& vals = (k==0) ? xs : ys;
				vd val(names.size());
				for (int j=0;j<(int)names.size();++j){
					val[j] = (int)cp.lined[idxs[j]];
					//if (!strcmpitr(names[j],"is_sell") || !strcmpitr(names[j],"is_buy"))
					//	val[j] *= (cp.lined[volidx]+1)/10.;
				}
				//if (cp.lined[volidx] >= 10 || cp.lined[volidx] == 0)
				vals.push_back(val);
			}
		}
		for (int i=(int)ys.size()-2;i>=0;--i){
			int atleast1one = 0;
			for (int j=0;j<(int)ys[i].size();++j){
				if (ys[i][j]!=0) atleast1one = 1;
			}
			if (!atleast1one)
				ys[i] = ys[i+1];
		}
		vvd xxs, yys;
		xxs.reserve(20000),yys.reserve(20000);
		for (int i=0;i<(int)xs.size();++i){
			int atleast1one = 0;
			for (int j=0;j<(int)xs[i].size();++j){
				if (xs[i][j]!=0 && aloneflag[j]) atleast1one = 1;
			}
			if (atleast1one){
				xxs.push_back(xs[i]);
				yys.push_back(ys[i]);
			}
		}
		// normalize data
		vd xsmax = xxs[0];
		for (int j=0;j<(int)xxs[0].size();++j){
			for (int i=0;i<(int)xxs.size();++i){
				xsmax[j] = max(xsmax[j],xxs[i][j]);
			}
		}
		/*
		for (int i=0;i<(int)xxs.size();++i){
			double a, b;
			a = xxs[i][0]; b = xxs[i][1];
			xxs[i][0] = (a+b) / 500.;
			xxs[i][1] = ((a+b)==0) ? 0 : a/(a+b);
		}*/
		
		for (int j=0;j<(int)xxs[0].size();++j){
			for (int i=0;i<(int)xxs.size();++i){
				if (xsmax[j] > 20){
					//xxs[i][j] = xxs[i][j] <= 1 ? -2 : log(xxs[i][j])-2;
					//xxs[i][j] = pow(xxs[i][j]/100,1/2.);
					xxs[i][j] /= 150;
				}
			}
		}
		// normalize data end
		return make_pair(xxs, yys);
	}

	void teach(const shared_ptr<LearningSystem>& ls){
		for (int i=0;i<(int)dataset[Training].size();++i){
#ifdef _USE_BOOST_
			boost::timer timer_;
#else
			chrono::time_point<chrono::system_clock> start, end;
			start = chrono::system_clock::now();
#endif
			pair<vvd, vvd> xydata = getDataFast(dataset[Training][i], ls->xnames(), ls->ynames());
#ifdef _USE_BOOST_
			std::cout << "elapsed time ? " << timer_.elapsed() << std::endl;
#else
			end = chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
			std::cout << "elapsed time ? " << elapsed_seconds.count() << std::endl;
#endif
			//timer_.restart();
			//getData(dataset[Training][i], ls->xnames(), ls->ynames());
			//std::cout << "slower ? " << timer_.elapsed() << std::endl;
			ls->adds(xydata.first,xydata.second);
		}
		ls->optimize();
	}

	void eval(const shared_ptr<LearningSystem>& ls){
		for (int k=0;k<3;++k){
			int ndata=0;
			errors[k] = 0;
			for (int i=0;i<(int)dataset[k].size();++i){
				pair<vvd, vvd> xydata = getDataFast(dataset[k][i], ls->xnames(), ls->ynames());
				pair<double, int> res = ls->errors(xydata.first,xydata.second);
				errors[k] += res.first;
				ndata += res.second;
			}
			errors[k] /= ndata;
		}
	}

	vs dataset[3];
	double errors[3];
};

#endif // teach_eval_scheme_h__


/*

static pair<vvi, vvi> getData(const string& filename, const vs& xnames, const vs& ynames){
vi xidxs(xnames.size(),-1), yidxs(ynames.size(),-1);
vvi xs; vvi ys;
xs.reserve(200000),ys.reserve(200000);
CsvParser cp(filename);
int m = cp.getline();
for (int k=0;k<2;++k){
const vs& names = (k==0) ? xnames : ynames;
vi& idxs = (k==0) ? xidxs : yidxs;
for (int j=0;j<(int)names.size();++j){
for (int i=0;i<m;++i){
if (!strcmpitr(names[j], cp.line[i]))
idxs[j] = i;
}
assert(idxs[j]>=0);
}
}
while (m = cp.getline()){
for (int k=0;k<2;++k){
const vs& names = (k==0) ? xnames : ynames;
vi& idxs = (k==0) ? xidxs : yidxs;
vvi& vals = (k==0) ? xs : ys;
vi val(names.size());
for (int j=0;j<(int)names.size();++j){
val[j] = atoi(cp.line[idxs[j]].c_str());
}
vals.push_back(val);
}
}
return make_pair(xs, ys);
}

*/