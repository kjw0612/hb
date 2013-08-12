#include "stdafx.h"
#include "VisWindow.h"
#include "Logger.h"
#include "DrawDaily.h"

#include <Windows.h>

#include <algorithm>
#include <map>
using namespace std;

static map<HWND, VisWindow *> visMap;
TCHAR visWindowClass[] = "visWindow";
extern HINSTANCE hInst;

ATOM VisRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= VisProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)NULL;
	wcex.lpszMenuName	= "";
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= visWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

namespace {

const int idDrawTimer = 10001;
const int idListBox = 10002;
void drawListBox(VisWindow *window) {
	RECT rect;
	GetClientRect(window->hWnd, &rect);

	HWND& hListBox = window->hListBox;
	if (hListBox == NULL) {
		hListBox =  CreateWindow("LISTBOX", NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_STANDARD | LBS_NOINTEGRALHEIGHT,
			0, 0, 90, rect.bottom - rect.top,
            window->hWnd,
            (HMENU) idListBox,
            hInst,
            NULL);

		SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
		for (int i=0;i<(int)window->draws.size();i++) {
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(window->draws[i]->getName().c_str()));
		}
		window->curSelectItem = -1;
	}
	MoveWindow(hListBox, 0, 0, 90, rect.bottom - rect.top, FALSE);
}

void drawContents(VisWindow *window, HDC hdc) {
	HBITMAP hBitmap = window->hDrawBitmap, hOldBitmap;
	RECT rect = window->getDrawsArea();
	if (hBitmap != NULL) {
		HDC hMemDC;
		hMemDC=CreateCompatibleDC(hdc);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	} else {
		FillRect(hdc,&rect,GetSysColorBrush(COLOR_WINDOW));
	}
}

void RefreshDraws(VisWindow *window) {
	if (window->curSelectItem < 0) return;

	DrawWindow *dWnd = window->draws[ window->curSelectItem ];
	dWnd->redrawBitmap();
	window->hDrawBitmap = dWnd->getBitmap();
}

void refreshShow(VisWindow *window) {
	RefreshDraws(window);
	drawListBox(window);
}

void refreshDrawsSelect(VisWindow *window) {
	window->curSelectItem = (int)SendMessage(window->hListBox, LB_GETCURSEL, 0, 0);
	RefreshDraws(window);
}

}

LRESULT CALLBACK VisProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	Document *doc;
	VisWindow *window;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	char buf[4096];

	GetClientRect(hWnd, &rect);
	if (visMap.find(hWnd) != visMap.end()) window = visMap[hWnd];
	else window = NULL;

	if (window != NULL) doc = window->document;
	else doc = NULL;

	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, idDrawTimer, 60, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (doc != NULL) {
			switch(doc->status) {
			case Document::Status::LOADING:
				sprintf_s(buf, sizeof(buf), "Loading %.2lf %%", doc->LoadingPhase);
				DrawText(hdc, buf, (int)strlen(buf), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			case Document::Status::LOAD_FAIL:
				sprintf_s(buf, sizeof(buf), "Loading %.2lf %%", doc->LoadingPhase);
				DrawText(hdc, buf, (int)strlen(buf), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			case Document::Status::SHOW:
				drawContents(window, hdc);
				break;
			}
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case idListBox:
			switch (wmEvent) {
			case LBN_SELCHANGE:
				refreshDrawsSelect(window);
				break;
			}
		default:
			break;
		}
		break;
	case WM_TIMER:
		if (wParam == idDrawTimer) {
			if (doc->status == Document::Status::SHOW) refreshShow(window);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case WM_DESTROY:
		visMap.erase(hWnd);
		KillTimer(hWnd, idDrawTimer);
		DestroyWindow(window->hListBox);
		window->remove();
		delete window;
		DestroyWindow(window->hWnd);

		break;
	case WM_SIZE:
		if (doc->status == Document::Status::SHOW)
			refreshShow(window);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


namespace {
	bool isFuture(string path) {
		string prefix = path.substr( path.find_last_of('\\')+1, 6 );
		return prefix == "KR4101";
	}
	bool isOption(string path) {
		string prefix = path.substr( path.find_last_of('\\')+1, 6 );
		return prefix == "KR4201" || prefix == "KR4301";
	}

	void InitDraws(VisWindow *Vwnd) {
		Document *doc = Vwnd->document;
		vector<DrawWindow *>& draws = Vwnd->draws;
		if (isFuture(doc->filepath) || isOption(doc->filepath)) {
			draws.push_back(new DrawDaily(Vwnd));
		}
	}
}
VisWindow::VisWindow(VisInfo *info, Document *doc, int id) : 
	hWnd(NULL), hListBox(NULL), hDrawBitmap(NULL), curSelectItem(-1)
{
	char buf[4096];

	visInfo= info;
	document = doc;
	myid = id;
	
	InitDraws(this);
	sprintf_s(buf, sizeof(buf), "[%d] %s", myid, doc->filepath.c_str());

	hWnd = CreateWindow(visWindowClass, buf , WS_OVERLAPPEDWINDOW,
      0, 0, 600, 400, NULL, NULL, hInst, NULL);

	visMap[hWnd] = this;
	ShowWindow(hWnd, SW_SHOW);
}


VisWindow::~VisWindow(void)
{
	for (int i=0;i<(int)draws.size();i++) {
		delete draws[i];
	}
}

void VisWindow::remove() {
	visInfo->removeWindow(myid);
}


RECT VisWindow::getDrawsArea(void)
{
	RECT res;
	GetClientRect(hWnd, &res);
	res.left = 90;
	res.bottom -= 15;
	return res;
}
