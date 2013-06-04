#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "brickbase.h"
#include "readerset.h"
#include "indexer.h"
#include "plotter.h"
#include "readerstatic.h"

#ifdef BOOST
#include <boost/timer.hpp>
#endif

typedef std::vector<int> v_i;
typedef std::vector<double> v_d;

void run(){
	int start_time = 9000000, end_time = 9590000;
	ReaderStatic::get().setupPath_1();
	ReaderStatic::get().setupDesc_2();
	ReaderStatic::get().setupIndex_3();

	Plotter plotter("A research for synthetic Indicator");

	ReaderStatic::get().setupDataTime_4(start_time, end_time);
	const std::vector<Brick *>& cgreek2550 = ReaderStatic::get().greeksbase().get("KR4201H52550");
	const std::vector<Brick *>& pgreek2550 = ReaderStatic::get().greeksbase().get("KR4301H52558");
	const std::vector<Brick *>& futbrick = ReaderStatic::get().futbase().get("KR4101H60001");
	const std::vector<Brick *>& cbrick2550 = ReaderStatic::get().callbase().get("KR4201H52550");
	const std::vector<Brick *>& pbrick2550 = ReaderStatic::get().putbase().get("KR4301H52558");

	std::pair<v_i, v_d>
		cg2550series = bricks2MidPriceGrid(cgreek2550, start_time, end_time, 5000, Brick::Delta);
	std::pair<v_i, v_d>
		pg2550series = bricks2MidPriceGrid(pgreek2550, start_time, end_time, 5000, Brick::Delta);
	std::pair<v_i, v_d>
		c2550series = bricks2MidPriceGrid(cbrick2550, start_time, end_time, 5000);
	std::pair<v_i, v_d>
		p2550series = bricks2MidPriceGrid(pbrick2550, start_time, end_time, 5000);
	std::pair<v_i, v_d>
		cMinusPseries = aXpbY(1, c2550series, -1, p2550series);
	std::pair<v_i, v_d>
		futSeries = bricks2MidPriceGrid(futbrick, start_time, end_time, 5000);
	std::pair<v_i, v_d>
		futShift = aXpb(1, futSeries ,-250);
	std::pair<v_i, v_d>
		futMcall = aXpbY(1, futShift, -1, cMinusPseries);

	plotter.addPlot(c2550series,"Call 255.0");
	plotter.addPlot(p2550series,"Put 255.0");
	plotter.addPlot(cMinusPseries,"Call 255.0 - Put 255.0");
	plotter.addPlot(futShift,"Future - 250");
	plotter.addPlot(futMcall,"Future - Synthetic Future");
	plotter.refreshForever();
	return;
	//for (int i=0;i<(int))
	/*
	FILE *fo = fopen("output.csv","wt");
	for (int i=0;i<(int)c2550series.first.size();++i){
		fprintf(fo,"%d,%lf\n",c2550series.first[i], c2550series.second[i]);
	}
	fclose(fo);*/
}
int main(){
	run();
	return 0;
}
/*
#include <iostream>
#include <windows.h>

long __stdcall WindowProcedure( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
{
    switch(msg)
    {
        case WM_DESTROY:
            std::cout << "\ndestroying window\n" ;
            PostQuitMessage(0) ;
            return 0L ;
        case WM_LBUTTONDOWN:
            std::cout << "\nmouse left button down at (" << LOWORD(lp)
                      << ',' << HIWORD(lp) << ")\n" ;
            // fall thru
        default:
            std::cout << '.' ;
            return DefWindowProc( window, msg, wp, lp ) ;
    }
}

int main()
{
    std::cout << "hello world!\n" ;
    const char* const myclass = "myclass" ;
    WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
                            0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
                            LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
                            0, myclass, LoadIcon(0,IDI_APPLICATION) } ;
    if( RegisterClassEx(&wndclass) )
    {
        HWND window = CreateWindowEx( 0, myclass, "title",
                   WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0 ) ;
        if(window)
        {
            ShowWindow( window, SW_SHOWDEFAULT ) ;
            MSG msg ;
            while( GetMessage( &msg, 0, 0, 0 ) ) DispatchMessage(&msg) ;
        }
    }
}
*/
