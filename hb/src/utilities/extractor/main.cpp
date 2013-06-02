#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "readers.h"
#include "indexer.h"
#include "pplot/PPlot.h"


DataWindow dw;
DescSet ds;
std::string basepath = "";

void makedesc(){
	//KospiFuturesReader futrdr("data/F191_15571");
	//KospiOptionsReader optrdr("data/C195_15511");
	MakeDesc fmd(basepath+"data\\F191_15571",'f');
	MakeDesc omd(basepath+"data\\C195_15511",'o');
	ds.append(fmd.getDS());
	ds.append(omd.getDS());
}

void setup(){
	std::ifstream fi("synconfig.txt");
	std::string a,b;
	while(!fi.eof()){
		fi >> a;
		std::getline(fi,b);
		while(b[0]==32){
			b = b.substr(1);
		}
		if (!_strcmpi(a.c_str(),"basepath")){
			basepath = b;
		}
	}
}

#include <windows.h>

class Plotter //: public NativeWidget, 	public Painter
{
public:
	Plotter() {
		const char g_szClassName[] = "myWindowClass";
		WNDCLASSEX wc;
		HWND hwnd;
		HINSTANCE hInstance = GetModuleHandle(NULL);
		MSG Msg;

		//Step 1: Registering the Window Class
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = 0;
		//wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = g_szClassName;
		wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

		if(!RegisterClassEx(&wc))
		{
			MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
			throw "";
		}
		
		fprintf(stderr, "%x\n", GetLastError());
		// Step 2: Creating the Window
		hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			g_szClassName,
			"The title of my window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
			NULL, NULL, hInstance, NULL);
		fprintf(stderr, "%x\n", GetLastError());

	    /* Calling GetDC with argument 0 retrieves the desktop's DC */
		HDC hDC_Desktop = GetDC(0);
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);

		/* Draw a simple blue rectangle on the desktop */
		RECT rect = { 20, 20, 200, 200 };
		HBRUSH blueBrush=CreateSolidBrush(RGB(0,0,255));
		FillRect(hDC_Desktop, &rect, blueBrush);
	}
	void addPlot(const std::pair<std::vector<int>, std::vector<double> >& data){
		PlotData *xdata = new PlotData ();
		PlotData *ydata = new PlotData ();
		// fill them as any stl container (with floats)
		for (int i=0;i<(int)data.first.size();++i) {
		   xdata->push_back ((float)data.first[i]);
		   ydata->push_back ((float)data.second[i]);
		}  
		// add item to the plot
		mPPlot.mPlotDataContainer.AddXYPlot (xdata, ydata);// takes ownership
	}
	PPlot mPPlot;
};

#undef min

void indexing(){
	Plotter plotter;
	/*
	Indexer indexer(basepath+"data\\C161_15515",'c');
	indexer.run();
	indexer.setIndexTree();
	//indexer.getQuestionTest();
	BlockReader<KospiOptionsReader> kobr(basepath+"data\\C161_15515",&indexer);
	std::vector<Brick *> bricks = kobr.readBlockTime(9000000,9200000);
	*/
	ReaderSet prdst(basepath+"P162_15516",'p');
	std::vector<Brick *> pbricks = prdst.blrd.readBlockTime(9000000,9200000);
	ReaderSet crdst(basepath+"C161_15515",'c');
	std::vector<Brick *> cbricks = crdst.blrd.readBlockTime(9000000,9200000);
	ReaderSet frdst(basepath+"F171_15572",'f');
	std::vector<Brick *> fbricks = frdst.blrd.readBlockTime(9000000,9200000);

	// target ATM = 250.5. month 2013-06.......
	BrickBase cbbase(cbricks);
	BrickBase pbbase(pbricks);
	BrickBase fbbase(fbricks);
	//cbbase.report_amounts();
	//pbbase.report_amounts();
	//fbbase.report_amounts();

	// 거래량 많은 종목
	//5월만기종목 set로..
	//KR4201H52550	11080		KR4301H52558	5042
	//KR4201H52527	9474		KR4301H52525	10984

	//6월만기종목 set로는..
	//KR4201H62526	3321		KR4301H62524	1078
	//KR4201H62559	4166		KR4301H62557	755

	//그러나 거래량을 감안하면 당연히 5월만기를 써야할듯.
	//5월만기로 작업 ^ㅡ^

	const std::vector<Brick *>& cbrick2550 = cbbase.get("KR4201H52550");
	const std::vector<Brick *>& pbrick2558 = pbbase.get("KR4301H52558");

	std::pair<std::vector<int>, std::vector<double> >
		c2550series = bricks2MidPriceGrid(cbrick2550, 9000000, 9200000, 5000);

	std::pair<std::vector<int>, std::vector<double> >
		p2558series = bricks2MidPriceGrid(pbrick2558, 9000000, 9200000, 5000);

	plotter.addPlot(c2550series);
	plotter.addPlot(p2558series);
	//for (int i=0;i<(int))

}

void sample(int fromtime = 9000000, int totime = 15000000){
	KospiOptionsReader crdr(basepath+"data\\C161_15515");
	KospiOptionsReader prdr(basepath+"data\\P162_15516");
	KospiFuturesReader frdr(basepath+"data\\F171_15572");
	//t_KrxOptionsBestQuotation = 1629858793,// JSHash(B6034,5) 
	//t_KrxOptionsTrade = 1611359895,// JSHash(A3034,5) 
	//t_KrxOptionsTradeBestQuotation = 1618535850,// JSHash(G7034,5) 
	//t_KrxOptionsEnd = 1612413213,// JSHash(A6034,5) 
	//t_KrxOptionsMarketManage = 1611938576,// JSHash(A7034,5) 
	//t_KrxOptionsSpace = 1618603596,// JSHash(G9034,5) 
	//t_KrxOptionsDistribute = 1696059202,// JSHash(O6034,5) 
	//t_KrxOptionsInvestorData = 1701177067,// JSHash(H1034,5) 
	//t_KrxOptionsUnsettled = 1701402316,// JSHash(H2034,5) 
	//t_KrxOptionsMargin = 1702452665,// JSHash(H3034,5) 
	//t_KrxOptionsMarketpriceRecovery = 1612008420,// JSHash(B2034,5) 
	//t_KrxOptionsPolling = 1637815616,// JSHash(I2034,5) 
	//t_KrxOptionsSettleBeforeOpen = 1623615942,// JSHash(M7034,5) 
	//t_KrxOptionsOpenMarketManage = 1623649775,// JSHash(M4034,5) 
	//t_KrxOptionsGreek = 1625671968,// JSHash(N7034,5) 
	//t_KrxOptionsImpvol = 1045394243,// JSHash(,5) 
	int cnt = 0;
	int timestampi = 0;
	double ask1price = 0, currentprice = 0;
	while(1){
		int mintime = std::min(std::min(crdr.timestamp, prdr.timestamp), frdr.timestamp);
		long long optionFutureType = 0;
		KrxOptionsHeader optheader;
		KrxFuturesHeader futheader;
		enum OnlyForWatch { CallOption, PutOption, Future } cpfType;
		int *tti;

		if (mintime==crdr.timestamp){
			crdr.next();
			optionFutureType = crdr.optionsType;
			optheader.m_rawmsg = crdr.content;
			tti = &crdr.timestamp;
			cpfType = CallOption;
		}
		else if (mintime==prdr.timestamp){
			prdr.next();
			optionFutureType = prdr.optionsType;
			optheader.m_rawmsg = prdr.content;
			tti = &prdr.timestamp;
			cpfType = PutOption;
		}
		else{
			frdr.next();
			optionFutureType = frdr.futuresType;
			futheader.m_rawmsg = frdr.content;
			tti = &frdr.timestamp;
			cpfType = Future;
		}

		char timestamp[20] = "", krcode[20] = "";
		switch(optionFutureType){
			case t_KrxFuturesTradeBestQuotation:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesTradeBestQuotation->timestamp);
				ask1price = ATOI_LEN(futheader.m_KrxFuturesTradeBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesTradeBestQuotation->krcode);
				break;
			case t_KrxOptionsTradeBestQuotation:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsTradeBestQuotation->timestamp);
				ask1price = ATOI_LEN(optheader.m_KrxOptionsTradeBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsTradeBestQuotation->krcode);
				break;
			case t_KrxFuturesTrade:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesTrade->timestamp);
				currentprice = ATOI_LEN(futheader.m_KrxFuturesTrade->currentprice) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesTrade->krcode);
				break;
			case t_KrxOptionsTrade:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsTrade->timestamp);
				currentprice = ATOI_LEN(optheader.m_KrxOptionsTrade->currentprice) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsTrade->krcode);
				break;
			case t_KrxFuturesBestQuotation:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->timestamp);
				ask1price = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesBestQuotation->krcode);
				break;
			case t_KrxOptionsBestQuotation:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->timestamp);
				ask1price = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsBestQuotation->krcode);
				break;
			default:
				continue;
		}
		if (fromtime <= timestampi && timestampi <= totime){
			if (optionFutureType==t_KrxOptionsTrade || optionFutureType==t_KrxFuturesTrade){
				dw.push(krcode, currentprice, timestampi, 1);
			}
			else{
				dw.push(krcode, ask1price, timestampi, 0);
			}
			*tti = timestampi;
			
			if ((++cnt % 10000) == 0){
				printf("%s   %d    %.2lf    %.2lf\n",krcode, timestampi, ask1price, currentprice );
			}
		}
		else if (totime < timestampi){
			return;
		}
	}
}

int main(){
	setup();
	makedesc();
	indexing();
	//sample();
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