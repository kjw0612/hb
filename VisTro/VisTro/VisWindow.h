#pragma once
class VisWindow;
class DrawWindow;

#include "VisInfo.h"
#include "Document.h"

#include <vector>
#include <string>

class VisWindow
{
public:
	VisWindow(VisInfo *info, Document *doc, int id);
	~VisWindow(void);

	void remove();

	VisInfo *visInfo;
	Document *document;
	int myid;

	HWND hWnd, hListBox;
	int curSelectItem;

	HBITMAP hDrawBitmap;

	xTime nowTime;
	
	std::vector<DrawWindow *> draws;
	RECT getDrawsArea(void);
};

class DrawWindow {
public:
	VisWindow *window;

	virtual void redrawBitmap()=0;
	virtual HBITMAP getBitmap()=0;
	virtual std::string getName()=0;
};

ATOM				VisRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	VisProc(HWND, UINT, WPARAM, LPARAM);

