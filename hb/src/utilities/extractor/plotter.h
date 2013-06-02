#ifndef PLOTTER_H_

#include <iostream>
#include "pplot/PPlot.h"
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

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


class MFCWidget 
	//: public Painter
	: public DummyPainter
{
public:
	MFCWidget() : width(800), height(600) {
		using namespace Gdiplus;
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;
		// Initialize GDI+.
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		const char g_szClassName[] = "myWindowClass";
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
			0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
			LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
			0, g_szClassName, LoadIcon(0,IDI_APPLICATION) } ;		
		HINSTANCE hInstance = GetModuleHandle(NULL);
		if(!RegisterClassEx(&wc))
		{
			MessageBox(NULL, "Window Registration Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			throw "";
		}
		// Step 2: Creating the Window
		hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			g_szClassName,
			"The title of my window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL, NULL, hInstance, NULL);
		ShowWindow( hwnd, SW_SHOWDEFAULT );
		hDC = GetDC(hwnd);
		graphics = new Graphics(hDC);
		blackpen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 255));
		blackbrush = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 255));
	}
	~MFCWidget(){
		delete graphics;
		delete blackpen;
		delete blackbrush;
	}

	/*
	{
	const char g_szClassName[] = "myWindowClass";
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
	0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
	LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
	0, g_szClassName, LoadIcon(0,IDI_APPLICATION) } ;		
	HINSTANCE hInstance = GetModuleHandle(NULL);
	if(!RegisterClassEx(&wc))
	{
	MessageBox(NULL, "Window Registration Failed!", "Error!",
	MB_ICONEXCLAMATION | MB_OK);
	throw "";
	}
	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
	WS_EX_CLIENTEDGE,
	g_szClassName,
	"The title of my window",
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, CW_USEDEFAULT, 600, 800,
	NULL, NULL, hInstance, NULL);
	ShowWindow( hwnd, SW_SHOWDEFAULT );
	//ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);
	hDC = GetDC(hwnd);
	}*/
	void DrawLine (float inX1, float inY1, float inX2, float inY2){
		graphics->DrawLine (blackpen,inX1, inY1, inX2, inY2);
	}
	void FillRect (int inX, int inY, int inW, int inH){
		graphics->FillRectangle(blackbrush, inX, inY, inW, inH);
	}
	void InvertRect (int inX, int inY, int inW, int inH){
		// sleepy..
	}
	void SetClipRect (int inX, int inY, int inW, int inH){
		// sleepy..
	}
	long GetWidth () const{
		return width;
	}
	long GetHeight () const{
		return height;
	}
	void SetLineColor (int inR, int inG, int inB){
		// sleepy..
	}
	void SetFillColor (int inR, int inG, int inB){
		// sleepy..
	}
	long CalculateTextDrawSize (const char *inString){
		return 12 * strlen(inString); // sorry..
	}
	long GetFontHeight () const {
		return 12; // sleepy..
	}

	void DrawText (int inX, int inY, const char *inString){
	}

	void DrawRotatedText (int inX, int inY, float inDegrees, const char *inString){
		 // sleepy..
	}
	void SetStyle (const PStyle &inStyle){
		 // sleepy..
	}
	int width, height;
	Gdiplus::Graphics *graphics;
	Gdiplus::Pen *blackpen;
	Gdiplus::SolidBrush *blackbrush;
	HWND hwnd;
	HDC hDC;
};


class Plotter : public MFCWidget
{
public:
	Plotter() {}
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

	void draw(){
		mPPlot.Draw(*this);
	}

	void testplot(){
		PlotData *xdata = new PlotData ();
		PlotData *ydata = new PlotData ();
		for (int i=0;i<100;++i) {
			xdata->push_back ((float)i);
			ydata->push_back ((float)i*i);
		}  
		// add item to the plot
		mPPlot.mPlotDataContainer.AddXYPlot (xdata, ydata);// takes ownership
		mPPlot.Draw(*this);
	}

	PPlot mPPlot;
};

#undef min


#endif // !PLOTTER_H_
