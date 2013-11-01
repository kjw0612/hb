#include "obinfo2sig.hpp"
#include "formaldata.hpp"
//#include "gnuplot/gnuplotpp.h"
#include "gnuplot/pGNUPlot.h"

namespace {
	string datesnew[19] = {"20130805", "20130806", "20130807", "20130808", "20130809", 
		"20130812", "20130813", "20130814", "20130816", "20130819",
		"20130820", "20130821", "20130822", "20130823", "20130826",
		"20130827", "20130828", "20130829", "20130830"};
}


using namespace std;
/*
using namespace gnuplotpp;
void plottest(){
	singleplot g;

	g.set("term png" );
	g.set() << gpset( "output", "\"demo1.png\"" ) << endl;

	g.x.label( "x axis" ).format( "%03.2f" ).tics(0.2).mtics(.05);
	g.y.label( "y axis" );

	g.label( "simgle plot example", coord( "graph 0.5", "graph 0.5" ) );

	g.arrow( coord( "graph 0.5", "graph 0.5" ), coord( "graph 1", "graph 1" ) );

	g.plotfile( "data.dat", 1, 2 )<< " t 'data'" << " pt 1" ;
  
	g.plotfunc( "x" ) ;

	g.save( "demo1.gp" ).exec();
}*/

// http://blog.daum.net/pg365/28


struct datter{
	void resize(int _n){
		n_ = _n; idx_ = vi(n_,0);
		end_ = _n;
		for (int i=0;i<n_;++i) idx_[i] = i;
	}
#define BASEPATH "D:\\PLOTDATA\\CRAZY\\"
	datter(int _n = 0, shared_ptr<CpGnuplot> _gplot = 
#ifdef _USE_BOOST_
		shared_ptr<CpGnuplot>(new CpGnuplot("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe"))
#else
		shared_ptr<CpGnuplot>(new CpGnuplot("C:\\Program Files (x86)\\gnuplot\\bin\\wgnuplot.exe"))
#endif
		) : n_(_n), st_(0), gplot_(_gplot), end_(_n)
	{
		check_and_create_directory(BASEPATH);
		resize(_n);
		gplot_->cmd("set terminal png size 1200,800 enhanced font \"Consolas,10\"");
		gplot_->cmd("set style line 1 lt 1 lw 1");
		gplot_->cmd("set grid");
		gplot_->cmd("set mxtics 15");
		gplot_->cmd("set mytics 15");
		gplot_->cmd("set grid mxtics mytics");
	}
	int getmin(){ return st_;}
	int getmax(){ return n_; }
	void setmin(int _st){ st_ = _st; }
	void setmini(const vd& rhs){
		for (int i=0;i<(int)rhs.size();++i){
			if (rhs[i]>0){
				setmin(i);
				break;
			}
		}
	}
	void setminmax(int _st, int _end){
		st_ = _st; end_ = _end;
	}
	template<class T>
	void adddat(const pair<vi, vector<T>>& dat, const std::string& str, double miny = -0.5, double maxy = 0.5)
	{
		adddat(dat.first,dat.second,str,miny,maxy);
	}
	template<class T>
	void adddat(const vi& idx, const vector<T>& rhs, const std::string& str, double miny = -0.5, double maxy = 0.5){
		names_.push_back(str);
		if (rhs.size() != idx.size()){
			printf("idx-data dimension mismatches");
			return;
		}
		if (n_==0){
			idx_ = idx;
			n_ = idx_.size();
			end_ = n_;
		}
		vd row(n_,0);
		for (int i=0;i<(int)idx.size();++i){
			for (int j=(i==0) ? 0:idx[i-1]+1; j<=idx[i]; ++j){
				row[j] = rhs[i];
			}
		}
		minmax_.push_back(std::make_pair(miny,maxy));
		dat_.push_back(row);
	}
	void print(std::string filename, vvd dat, const vs& names, int print_index = 1){
		filename = BASEPATH + filename;
		FILE *fo = NULL;
		fopen_s(&fo, filename.c_str(), "wt");

		for (int j=0;j<(int)dat.size();++j){
			if (minmax_[j].first != Null<double>())
				setMinMax(dat[j].begin()+st_,dat[j].begin()+min(end_,n_), minmax_[j].first, minmax_[j].second);
		}

		/*
		if (print_index) fprintf(fo,"idx ");
		for (int j=0;j<(int)dat.size();++j) fprintf(fo,"%s ",names[j].c_str());
		fprintf(fo,"\n");*/

		for(int i=st_;i<min(end_,n_);++i){
			if (print_index)
				fprintf(fo,"%d ",idx_[i]);
			for (int j=0;j<(int)dat.size();++j){
				fprintf(fo,"%lf ",dat[j][i]);
			}
			fprintf(fo,"\n");
		}
		fflush(fo);
		fclose(fo);
	}
	void plot(std::string filename, int ns){
		string setoutput = string("set output \'") + BASEPATH + filename + ".png";
		gplot_->cmd(setoutput.c_str());

		filename = BASEPATH + filename;
		string command = "plot ";
		for (int i=0;i<ns;++i){
			char buf[10+1] = "";
			sprintf_s(buf,10,"%d",i+2);
			command += "\'" + filename + "\' using 1:" + buf +  " with lines";
			command += " title \'" + names_[i] + "\'";
			if (i!=ns-1) command += ",\\";
			printf("%s\n",command.c_str());
			gplot_->cmd(command.c_str());
			command = "";
		}
		command += " \n";
		Sleep(5000);
		//scanf_s("%c");
	}
	void clear(){
		dat_.clear();
		names_.clear();
		minmax_.clear();
	}
	void print_and_plot(const std::string& filename){
		print(filename, dat_, names_);
		plot(filename, dat_.size());
	}

	void print_and_plot_xy(const vd& xs, const vd& ys, string xname, string yname, const std::string& filename){
		vs names; vvd dt;
		dt.push_back(xs); dt.push_back(ys);
		names.push_back(xname); names.push_back(yname);
		print(filename, dt, names, 0);
		plot(filename, dt.size());
	}
#undef BASEPATH
private:
	int st_, end_;
	int n_;
	vi idx_;
	vvd dat_;
	vector<pair<double,double> > minmax_;
	vs names_;
	shared_ptr<CpGnuplot> gplot_;
};

#include "optimization_method.hpp"

class SimplePolyCF : public CostFunction{
public:
	SimplePolyCF(int _n) : n(_n), param(_n+1,0), calculated(0) {}
	
	static double polyFunction(double xval, const vd& poly){
		double ret = 0, yval = 0, multip = 1;
		for (int j=0;j<(int)poly.size();++j){
			yval = yval + multip * poly[j];
			multip *= xval;
		}
		return ret;
	}

	double costFunction(const vd& _param) const {
		double ret = 0;
		for (int i=0;i<(int)x.size();++i){
			double yval = polyFunction(x[i], _param);
			ret += pow(yval - y[i],2);
		}
		return ret;
	}

	void setData(const vd& _x, const vd& _y){ x = _x; y = _y; }

	void calculate(){
		if (!calculated){
			calculated = true;
			GradientDescent gd;
			gd.minimize(*this, param);
			param = gd.param;
		}
	}

	vd getParam(){
		calculate();
		return param;
	}

	vd getFittedData(){
		calculate();
		vd ret(x.size());
		for (int i=0;i<(int)x.size();++i){
			ret[i] = polyFunction(x[i], param);
		}
		return ret;
	}

	vd x, y, param;
	int n, calculated;
};

void rmsepoly(const vd& a, const vd& b){
}

void pplottest(){
	//CpGnuplot plot ("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe");
#ifdef _USE_BOOST_
	CpGnuplot plot ("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe");
#else
	CpGnuplot plot ("C:\\Program Files (x86)\\gnuplot\\bin\\wgnuplot.exe");
#endif
    // Gnuplot의 splot 명령을 사용하여 그래프를 그린다.
	plot.cmd ("splot [x=-3:3] [y=-3:3] sin(x) * cos(y)");
	scanf_s("%c");
}

void pnlanal(){
	// which volumer is worth follow?
	datter dt;
	for (int i=0;i<1;++i){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[i]));
		ObDataBase ob(dp.first, dp.second);
		vi buckets;
		buckets.push_back(1);buckets.push_back(2);buckets.push_back(3);buckets.push_back(5);buckets.push_back(10);
		buckets.push_back(20);buckets.push_back(30);buckets.push_back(40);buckets.push_back(50);buckets.push_back(100);buckets.push_back(500);
		for (int j=0;j+1<(int)buckets.size();++j){
			int minvol = buckets[j], maxvol = buckets[j+1]-1;
			char namebuf[201] = "";
			sprintf_s(namebuf,200,"pnl of volume range(%d to %d)",minvol,maxvol);
			dt.adddat(ob.getpnls(ob.idx_volminmax(minvol, maxvol)),namebuf,Null<double>(),Null<double>());
		}
		dt.print_and_plot("pnl" + datesnew[i]);
		dt.clear();
	}
}

void crazy_analysis(){
	datter dt;
	for (int di=0;di<1;++di){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[di]));
		ObDataBase ob(dp.first, dp.second);
		dt.resize(ob.size());
		pair<vi, vd> wmp = ob.wmprices(), accumtot = ob.accumqty();
		dt.setmini(wmp.second);
		dt.adddat(wmp, "weighted midprices");
		dt.adddat(accumtot,"accumqty of trades.");
		//ob.idx_set(set_intersection(ob.idx_conseqadj(50), ob.idx_volminmax(1,5)));
		//pair<vi, vd> accumspec = ob.accumqty();
		//dt.adddat(accumspec,"accumqty of consq trades.");
		dt.adddat(ob.tdensity(accumtot.first,50),"density of trades.");
		dt.adddat(ob.acclimit(1,0),"accum limit buy.");
		dt.adddat(ob.acclimit(0,1),"accum limit sell.");
		char buf[101] = "";
		int n=0;
		int width = 5000;
		for(int i=dt.getmin();i<dt.getmax();i+=width,++n){
			dt.setminmax(i,i+width); // to be modified.
			sprintf_s(buf,100,"crazy_%s_%d",datesnew[di].c_str(),n);
			dt.print_and_plot(buf);
		}
	}
}

void caseaccumanal(){
	//plottest();
	//return;
	datter dt;
	//dt.plot("asdfadf",4);
	for (int di=0;di<1;++di){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[di]));
		ObDataBase ob(dp.first, dp.second);
		dt.resize(ob.size());
		int imin = 0;
		pair<vi, vd> wmp = ob.wmprices();
		for (int j=0;j<(int)wmp.second.size();++j){
			if (wmp.second[j]>0){
				imin = j;
				setMinMax(wmp.second.begin()+imin,wmp.second.end(),1.,2.);
				dt.setmin(j);
				break;
			}
		}

		printf("tic");
		dt.adddat(wmp, "weighted midprices");

		
		//dt.adddat(ob.accumqty_conseq(5,1,5), "conseq adjacent accumqty small(1~5)"); // 10ms
		//dt.adddat(ob.pnls, "conseq adjacent pnl small(1~5)");
		ob.idx_set(set_union(ob.idx_conseqadj(50), ob.idx_volminmax(1,5)));
		dt.adddat(ob.tdensity(ob.accumqty().first,50),"density of it.");
		//dt.adddat(ob.accumqty_conseq(5,11,500), "conseq adjacent accumqty large(11~500)"); // 10ms
		//dt.adddat(ob.pnls, "conseq adjacent pnl large(11~500)");
		printf("toc");
		dt.print_and_plot("case_accumqtys" + datesnew[di]);
		/*
		dt.adddat(ob.accumqty_before_pricemove(50,1,5), "before pricemove adjacent accumqty small(1~5)"); // 10ms
		dt.adddat(ob.pnls, "before pricemove adjacent pnl small(1~5)");
		dt.adddat(ob.accumqty_before_pricemove(50,50,500), "before pricemove adjacent accumqty large(50~500)"); // 10ms
		dt.adddat(ob.pnls, "before pricemove adjacent pnl large(50~500)");
		dt.print_and_plot("fast_accumqtys" + datesnew[di]);
		*/
		dt.clear();
	}
}

void pianal(){
	//plottest();
	//return;
	datter dt;
	//dt.plot("asdfadf",4);
	for (int di=0;di<11;++di){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[di]));
		ObDataBase ob(dp.first, dp.second);
		dt.resize(ob.size());
		int imin = 0;
		pair<vi, vd> wmp = ob.wmprices();
		for (int j=0;j<(int)wmp.second.size();++j){
			if (wmp.second[j]>0){
				imin = j;
				setMinMax(wmp.second.begin()+imin,wmp.second.end(),1.,2.);
				dt.setmin(j);
				break;
			}
		}
		dt.adddat(wmp, "weighted midprices");
		pair<vi, vd> accumqty = ob.accumqty();
		setMinMax(accumqty.second.begin()+imin,accumqty.second.end(),1.,2.);
		dt.adddat(accumqty, "all accum signed qtys");
		vi xidxs;
		vd xs, ys;
		int dn = 1000;
		int ddn = 200;
		for (int i=0;i<(int)wmp.first.size();++i){
			double acmdelta = 0;
			double wmpdelta = 0;
			if (wmp.second[i] > 0 && i-2*dn >=0 && wmp.second[i-2*dn]>0){
				for (int j=i-dn+ddn;j<i;j+=ddn){
					acmdelta += fabs(accumqty.second[j] - accumqty.second[j-ddn]);
					wmpdelta += fabs(wmp.second[j] - wmp.second[j-ddn]);
					//acmdelta += fabs(accumqty.second[i] - accumqty.second[j]);
					//wmpdelta += fabs(wmp.second[i] - wmp.second[j]);
					/*
					if (fabs(acmdelta) < fabs(accumqty.second[i] - accumqty.second[j]))
						acmdelta = accumqty.second[i] - accumqty.second[j];
					if (fabs(wmpdelta) < wmp.second[i] - wmp.second[j])
						wmpdelta = wmp.second[i] - wmp.second[j];
						*/
				}
				xidxs.push_back(wmp.first[i]);
				//xs.push_back(wmp.first[i]);
				ys.push_back(wmpdelta / acmdelta);
			}
		}
		//dt.print_and_plot_xy(accumqty.second, wmp.second, "accum qtys", "weighted midprices", "xyplot");
		//dt.adddat(xidxs, ys);

		//dt.print_and_plot("simple");
		dt.clear();

		dt.adddat(wmp, "weighted midprices");
		//dt.adddat(ob.accumqty(), "all accum signed qtys");
		dt.adddat(ob.accumqty(ob.idx_volminmax(1,2)), "small accum signed qtys(1to2)");
		dt.adddat(ob.accumqty(ob.idx_volminmax(10,30)), "mid accum signed qtys(10to30)");
		//dt.adddat(ob.accumqty(20,30), "large accum signed qtys(20to30)");
		dt.adddat(ob.accumqty(ob.idx_volminmax(40,300)), "verylarge accum signed qtys(40to300)");
		//dt.adddat(ob.accumqty(50,300), "large accum signed qtys(50to300)");

		pair<vi, vd> acabs = ob.accumqtyabs(), acabsmalls = ob.accumqtyabs(ob.idx_volminmax(1,2)),
			acabmids = ob.accumqtyabs(ob.idx_volminmax(10,20)), acablarges = ob.accumqtyabs(ob.idx_volminmax(50,300));

		//dt.adddat(acabsmalls, "small accum unsigned qtys(1to2)",0,acabsmalls.second.back()/acabs.second.back());
		//dt.adddat(acablarges, "mid accum unsigned qtys(10to20)",0,acabmids.second.back()/acabs.second.back());
		//dt.adddat(acablarges, "large accum unsigned qtys(50to300)",0,acablarges.second.back()/acabs.second.back());
		dt.print_and_plot("accumqtys" + datesnew[di]);
		//dt.adddat(xidxs,ys);
		//dt.adddat(ob.accumqtyabs());
		//dt.adddat(ob.wmpdeltas(5000));
		//SimplePolyCF spcf(3);
		//spcf.setData(xs,ys);
		//dt.adddat(xidxs,spcf.getFittedData());
		
		//dt.adddat(ob.accumqtyabsavg(5000), "accum avg unsigned qtys");
		//dt.print_and_plot("simple2");
		dt.clear();
	}
}