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
		for (int i=0;i<n_;++i) idx_[i] = i;
	}
#define BASEPATH "D:\\PLOTDATA\\"
	datter(int _n = 0) : n_(_n), st_(0)
#ifdef _USE_BOOST_
		, gplot_ (new CpGnuplot("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe"))
#else
		, gplot_ (new CpGnuplot("C:\\Program Files (x86)\\gnuplot\\bin\\wgnuplot.exe"))
#endif
	{
		check_and_create_directory(BASEPATH);
		resize(_n);
		gplot_->cmd("set terminal png size 800,600 enhanced font \"Consolas,10\"");
		string setoutput = string("set output \'") + BASEPATH + "output.png" + "\'";
		gplot_->cmd(setoutput.c_str());
		gplot_->cmd("set style line 1 lt 1 lw 1");
	}
	void setmin(int _st){ st_ = _st; }
	template<class T>
	void adddat(const pair<vi, vector<T>>& dat)
	{
		adddat(dat.first,dat.second);
	}
	template<class T>
	void adddat(const vi& idx, const vector<T>& rhs){
		if (rhs.size() != idx_.size()){
			printf("idx-data dimension mismatches");
			return;
		}
		vd row(n_,0);
		for (int i=0;i<(int)idx.size();++i){
			for (int j=(i==0) ? 0:idx[i-1]+1; j<=idx[i]; ++j){
				row[j] = rhs[i];
			}
		}
		dat_.push_back(row);
	}
	void print(std::string filename){
		filename = BASEPATH + filename;
		FILE *fo = NULL;
		fopen_s(&fo, filename.c_str(), "wt");

		vvd dat = dat_;

		for (int j=0;j<(int)dat.size();++j)
			setMinMax(dat[j].begin()+st_,dat[j].begin()+n_, -0.5, 0.5);

		for(int i=st_;i<n_;++i){
			fprintf(fo,"%d ",idx_[i]);
			for (int j=0;j<(int)dat.size();++j){
				fprintf(fo,"%lf ",dat[j][i]);
			}
			fprintf(fo,"\n");
		}
		fclose(fo);
	}
	void plot(std::string filename, int ns){
		filename = BASEPATH + filename;
		string command = "plot ";
		for (int i=0;i<ns;++i){
			char buf[10+1] = "";
			sprintf_s(buf,10,"%d",i+2);
			command += "\'" + filename + "\' using 1:" + buf +  " with lines";
			if (i!=ns-1) command += ",\\";
			printf("%s\n",command.c_str());
			gplot_->cmd(command.c_str());
			command = "";
		}
		command += " \n";
		Sleep(5000);
		//scanf_s("%c");
	}
	void print_and_plot(std::string filename){
		print(filename);
		plot(filename, dat_.size());
	}
#undef BASEPATH
private:
	int st_;
	int n_;
	vi idx_;
	vvd dat_;
	shared_ptr<CpGnuplot> gplot_;
};

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

void pianal(){
	//plottest();
	//return;
	datter dt;
	//dt.plot("asdfadf",4);
	for (int i=0;i<1;++i){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[i]));
		ObDataBase ob(dp.first, dp.second);
		dt.resize(ob.size());
		pair<vi, vd> wmp = ob.wmprices();
		for (int j=0;j<(int)wmp.second.size();++j){
			if (wmp.second[j]>0){
				dt.setmin(j);
				break;
			}
		}
		dt.adddat(wmp);
		dt.adddat(ob.accumqty());
		dt.print_and_plot("simple");
	}
}