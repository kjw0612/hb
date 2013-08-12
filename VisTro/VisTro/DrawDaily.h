#pragma once
class DrawDaily;
#include "viswindow.h"

#include <string>


class DrawDaily :
	public DrawWindow
{
public:
	enum Status {
		LOADING, 
		LOAD_FAIL,
		SHOW,
	};

	DrawDaily(VisWindow *);
	~DrawDaily(void);

	void redrawBitmap();
	HBITMAP getBitmap();
	std::string getName();

	HBITMAP hBitmap, sBitmap;
	RECT hBitRect, sBitRect;
	VisWindow * window;

	Status status;
	double LoadingPhase;
	
	std::vector<double> price, pricemn, pricemx;

	void refreshSBitmap(RECT rect);

	bool loaded;
};

