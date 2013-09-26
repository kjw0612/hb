#ifndef teach_eval_scheme_h__
#define teach_eval_scheme_h__

#include "settings.hpp"
#include "csvparser.hpp"
#include "csvparserfast.hpp"


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
template<class vt>
vt& concat(vt& v1, vt& v2){
	v1.insert( v1.end(), v2.begin(), v2.end() );
	return v1;
}

class LearningSystem{
public:
	virtual vs xnames() = 0;
	virtual vs ynames() {
		vs ret; ret.push_back("dir_bidmoved"); ret.push_back("dir_askmoved");
		return ret;
	}
	virtual void add(const vi& x, const vi& y) = 0;
	virtual void adds(const vvi& xs, const vvi& ys) {
		assert(xs.size()==ys.size());
		for (int i=0;i<(int)xs.size();++i) add(xs[i],ys[i]);
	}
	virtual void optimize() = 0;
	virtual double error(const vi& x, const vi& y) = 0;
	virtual pair<double, int> errors(const vvi& xs, const vvi& ys) {
		double ret = 0;
		for (int i=0;i<(int)xs.size();++i) ret += error(xs[i], ys[i]);
		return make_pair(ret, (int)xs.size());
	}
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
	static pair<vvi, vvi> getDataFast(const string& filename, const vs& xnames, const vs& ynames){
		vi xidxs(xnames.size(),-1), yidxs(ynames.size(),-1);
		vvi xs; vvi ys;
		xs.reserve(200000),ys.reserve(200000);
		CsvParserFast cp(filename);
		int m = cp.getline(1);
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
		while (m = cp.getline(0)){
			for (int k=0;k<2;++k){
				const vs& names = (k==0) ? xnames : ynames;
				vi& idxs = (k==0) ? xidxs : yidxs;
				vvi& vals = (k==0) ? xs : ys;
				vi val(names.size());
				for (int j=0;j<(int)names.size();++j){
					val[j] = (int)cp.lined[idxs[j]];
				}
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
		vvi xxs, yys;
		xxs.reserve(20000),yys.reserve(20000);
		for (int i=0;i<(int)xs.size();++i){
			int atleast1one = 0;
			for (int j=0;j<(int)xs[i].size();++j){
				if (xs[i][j]!=0) atleast1one = 1;
			}
			if (atleast1one){
				xxs.push_back(xs[i]);
				yys.push_back(ys[i]);
			}
		}
		return make_pair(xxs, yys);
	}

	void teach(const shared_ptr<LearningSystem>& ls){
		for (int i=0;i<(int)dataset[Training].size();++i){
			boost::timer timer_;
			pair<vvi, vvi> xydata = getDataFast(dataset[Training][i], ls->xnames(), ls->ynames());
			std::cout << "faster ? " << timer_.elapsed() << std::endl;
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
				pair<vvi, vvi> xydata = getDataFast(dataset[k][i], ls->xnames(), ls->ynames());
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