#ifndef PLOTTER_H_

#include <iostream>
#include "pplot/PPlot.h"
#include "readerstatic.h"
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

inline long __stdcall WindowProcedure( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
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

inline std::wstring 
mbs_to_wcs(std::string const& str, std::locale const& loc = std::locale())
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = 0;
	std::vector<wchar_t> buf(str.size() + 1);
	char const* in_next = str.c_str();
	wchar_t* out_next = &buf[0];
	codecvt_t::result r = codecvt.in(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	return std::wstring(&buf[0]);
}

inline std::string 
wcs_to_mbs(std::wstring const& str, std::locale const& loc = std::locale())
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = 0;
	std::vector<char> buf((str.size() + 1) * codecvt.max_length());
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	codecvt_t::result r = codecvt.out(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	return std::string(&buf[0]);
}

using namespace Gdiplus;

class MFCWidget : public Painter
	//: public DummyPainter
{
public:
	MFCWidget(const std::string& windowname = "Default Window") : width(800), height(600) {
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
			windowname.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL, NULL, hInstance, NULL);
		ShowWindow( hwnd, SW_SHOWDEFAULT );
		hDC = GetDC(hwnd);
		graphics = new Graphics(hDC);
		current_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 255));
		current_brush = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 255));
		font = new Font(L"Consolas", 12, FontStyleRegular, UnitPixel);
	}
	~MFCWidget(){
		delete graphics;
		delete current_pen;
		delete current_brush;
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
		graphics->DrawLine (current_pen, inX1, inY1, inX2, inY2);
	}
	void FillRect (int inX, int inY, int inW, int inH){
		graphics->FillRectangle(current_brush, inX, inY, inW, inH);
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
		delete current_pen;
		current_pen = new Pen(Color(inR, inG, inB));
		current_brush = new SolidBrush(Color(inR, inG, inB));
		// sleepy..
	}
	void SetFillColor (int inR, int inG, int inB){
		delete current_brush;
		//printf("[%d,%d,%d]",inR,inG,inB);
		current_brush = new SolidBrush(Color(inR, inG, inB));
		// sleepy..
	}
	long CalculateTextDrawSize (const char *inString){
		return 12 * strlen(inString); // sorry..
	}
	long GetFontHeight () const {
		return 12; // sleepy..
	}
#undef DrawText
	void DrawText (int inX, int inY, const char *inString){
		std::wstring wInString = mbs_to_wcs(inString);
		graphics->DrawString(wInString.c_str(), wInString.length(), font, PointF((float)inX, (float)inY), current_brush);
	}

	void DrawRotatedText (int inX, int inY, float inDegrees, const char *inString){
		 // sleepy..
	}
	void SetStyle (const PStyle &inStyle){
		 // sleepy..
	}
	int width, height;
	Gdiplus::Graphics *graphics;
	Gdiplus::Pen *current_pen;
	Gdiplus::SolidBrush *current_brush;
	Gdiplus::Font *font;
	HWND hwnd;
	HDC hDC;
};

class Plotter : public MFCWidget
{
public:
	Plotter(const std::string& windowname = "Default Window") : windowname(windowname), colorSeed(42), MFCWidget(windowname) {}
	int colorSeed;
	const std::string windowname;

	PColor genColor() {
		srand(colorSeed);
		colorSeed = rand();
		return PColor(rand()%220,rand()%220,rand()%220);
		//return PColor(rand()%256,rand()%256,rand()%256);
	}

	void addPlot(const std::pair<std::vector<int>, std::vector<double> >& data, const std::string& legend, bool writeToText = true){
		PlotData *xdata = new PlotData ();
		PlotData *ydata = new PlotData ();
		// fill them as any stl container (with floats)
		for (int i=0;i<(int)data.first.size();++i) {
			//xdata->push_back (i);
			//ydata->push_back (i*2);
			xdata->push_back ((float)data.first[i]);
			ydata->push_back ((float)data.second[i]);
		}
		// add item to the plot
		LegendData *legend_data = new LegendData();
		legend_data->mName = legend;
		legend_data->mColor = genColor();
		legend_data->mStyle.mFont = "Consolas";
		
		mPPlot.mPlotDataContainer.AddXYPlot (xdata, ydata, legend_data);// takes ownership

		if (writeToText){
			FILE *fo;
			if (ReaderStatic::get().tempfile() == NULL){
				fopen_s(&ReaderStatic::get().tempfile(),"plotfile.csv","wt");
				fo = ReaderStatic::get().tempfile();
				fprintf(fo,"time,");
				for (int i=0;i<(int)data.first.size();++i)
					fprintf(fo,"%d,",data.first[i]);
				fprintf(fo,"\n");
			}
			fo = ReaderStatic::get().tempfile();
			fprintf(fo,"%s,",legend.c_str());
			for (int i=0;i<(int)data.first.size();++i)
				fprintf(fo,"%lf,",data.second[i]);
			fprintf(fo,"\n");
		}
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

	void refreshForever(){
		
		MSG msg;
		while( GetMessage( &msg, 0, 0, 0 ) ){
			draw();
			DispatchMessage(&msg) ;
		}
	}

	PPlot mPPlot;
};

#undef min


#endif // !PLOTTER_H_
