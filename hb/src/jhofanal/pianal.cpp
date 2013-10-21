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

void pplottest(){
	//CpGnuplot plot ("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe");
	CpGnuplot plot ("C:\\Program Files (x86)\\gnuplot\\bin\\wgnuplot.exe");
    // Gnuplot의 splot 명령을 사용하여 그래프를 그린다.
    plot.cmd ("splot [x=-3:3] [y=-3:3] sin(x) * cos(y)");
}

void pianal(){
	//plottest();
	pplottest();
	scanf("%c");
	return;
	for (int i=0;i<1;++i){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[i]));
		ObDataBase ob(dp.first, dp.second);
	}
}