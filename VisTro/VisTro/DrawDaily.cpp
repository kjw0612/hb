#include "stdafx.h"
#include "DrawDaily.h"
#include "Document.h"
#include "VisWindow.h"
#include "MarketDataLine.h"

#include <algorithm>
#include <string>

using namespace std;

namespace {

double getPrice(MarketDataLine *mdl) {
	if (mdl == NULL) return 0;
	else if (!mdl->ask.isEmpty() && !mdl->bid.isEmpty()) {
		return (mdl->ask.V[0].Prc + mdl->bid.V[0].Prc) / 2.0;
	} else {
		return mdl->expectedPrice;
	}
}

DWORD WINAPI ThreadLoading( LPVOID lpParam ) {
	DrawDaily *drawDaily = (DrawDaily *)lpParam;
	drawDaily->LoadingPhase = 0.0;
	drawDaily->status = DrawDaily::Status::LOADING;
	VisWindow *Vwnd = drawDaily->window;
	Document *doc = Vwnd->document;
	
	if (doc->mdls.empty()) {
		drawDaily->status = DrawDaily::Status::LOAD_FAIL;
		return 0;
	}
	xTime firstTime, lastTime, nowTime;
	firstTime = lastTime = doc->mdls[0]->arriveTime;
	firstTime.setTime("080000");
	lastTime.setTime("151500");
	vector<double>& price = drawDaily->price;
	vector<double>& pricemn = drawDaily->pricemn;
	vector<double>& pricemx = drawDaily->pricemx;
	price.clear();

	int nprice = 0;
	LL firtime = firstTime.getTime();
	LL alltime = lastTime.getTime() - firtime;

	int bidx = 0;
	for (nowTime = firstTime; nowTime.getTime() <= lastTime.getTime(); nowTime.sec += 10) { // 8시부터 10초단위로
		int idx = doc->upperBound(nowTime);
		MarketDataLine *mdl = (idx<0) ? NULL : doc->mdls[idx];

		double iprc = getPrice(mdl);
		price.push_back(iprc);
		double mnprc, mxprc;
		mnprc = mxprc = 0;
		for (int i=min(bidx+1, idx);i<=idx;i++) {
			MarketDataLine *mdl = (i<0) ? NULL : doc->mdls[i];
			double prc = getPrice(mdl);

			if (prc ==0 )continue;
			
			if (mnprc == 0 || mnprc > prc) mnprc = prc;
			if (mxprc == 0 || mxprc < prc) mxprc = prc;
		}
		pricemn.push_back(mnprc);
		pricemx.push_back(mxprc);

		LL nowtime = nowTime.getTime() - firtime;
		drawDaily->LoadingPhase = (double)nowtime / (double)alltime;
		bidx = idx;
	}

	drawDaily->status = DrawDaily::Status::SHOW;

	return 0;
}
}

DrawDaily::DrawDaily(VisWindow *Vwnd) 
	: hBitmap(NULL), window(Vwnd), loaded(false)
{
}


DrawDaily::~DrawDaily(void)
{
}

int getY(int mn,int mx,double y,int top, int bottom) {
	int height = bottom - top - 1;
	return top + (int)((double)height - ((double)(y - mn)* (double) height / (double)(mx-mn)));
}

void drawLine(HDC hdc, int x1,int y1, int x2, int y2, int style, COLORREF c) {
	HPEN OldPen, hPen;
	hPen = CreatePen(style,1, c);
	OldPen = (HPEN)SelectObject(hdc, hPen);
	
	MoveToEx(hdc, x1, y1, NULL); 
	LineTo(hdc, x2, y2);
	SelectObject(hdc, OldPen);
	DeleteObject(hPen);
}
void DrawDaily::refreshSBitmap(RECT rect) {
	char buf[4096];

	if (price.empty()) return;
	if (sBitmap == NULL || memcmp(&rect, &sBitRect, sizeof(RECT))) {
		sBitRect = rect;
		HWND hWnd = window->hWnd;
		HDC hdc = GetDC(hWnd), hMemDC;
		HBITMAP OldBit;
		if (sBitmap != NULL) {
			DeleteObject(sBitmap);
		}
		sBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		hMemDC = CreateCompatibleDC(hdc);
		OldBit = (HBITMAP)SelectObject(hMemDC, sBitmap);
		FillRect(hMemDC,&rect,GetSysColorBrush(COLOR_WINDOW));

		double min = -1, max = -1;
		for (int i=0;i<(int)price.size();i++) {
			double prc = price[i];
			if (prc == 0) continue;
			if (min < 0 || min > prc) min = prc;
			if (max < 0 || max < prc) max = prc;
		}

		int mn =(int) min, mx =(int) (max + 0.9);

		int units[] = {5, 10, 25, 50, 100, 250, 500};
		int Unit = 1;
		for (int i=0; i < sizeof(units) / sizeof(int); i++) {
			int unit = units[i];
			if ((mx - mn) / unit > 5) {
				Unit = unit;
			}
		}
		mn = (mn / Unit) * Unit - Unit;
		mx = (mx + Unit - 1) / Unit * Unit + Unit;
		int dst = mx - mn;
		
		int sx, ex, lx;
		sx = 70; ex = rect.right - 10;
		lx = ex - sx;

		int gtop, gbottom;
		gtop = rect.top + 10; gbottom = rect.bottom - 10;

		for (int i=mn;i<=mx;i+=Unit) {
			int y = (int)getY(mn, mx, i, gtop, gbottom);
			drawLine(hMemDC, sx, y, ex, y, PS_DOT, RGB(150, 150, 150));
			RECT prect = {10, y-10, sx, y+10};
			sprintf_s(buf, sizeof(buf), "%d.%02d", i/100,i%100);
			DrawText(hMemDC, buf, (int)strlen(buf), &prect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}


		double bpr = 0;
		int bidx = -360;

		xTime firstTime, nowTime;
		for (int x=sx;x<=ex;x++) {
			int dx = x - sx;
			int idx = dx * ((int)price.size()-1) / lx;

			if (idx / 360 != bidx / 360 || x == ex) {
				drawLine(hMemDC, x, 0, x, rect.bottom, PS_DASH, RGB(150, 150, 150));
			} else if (idx / 60 != bidx / 60) {
				drawLine(hMemDC, x, 0, x, rect.bottom, PS_DOT, RGB(200, 200, 200));
			}


			double pr = price[ idx ];
			double prmn = pricemn[ idx ];
			double prmx = pricemx[ idx ];
			
			for (int t = bidx + 1; t < idx; t ++) {
				if (0 <= t && t < (int)price.size()) {
					if (pricemn[t] < prmn) prmn = pricemn[t];
					if (pricemx[t] > prmx) prmx = pricemx[t]; 
				}
			}

			if (pr == 0.0) pr = (prmn+prmx)/2.0;
			if (pr != 0.0) {
				int y, by, mny, mxy;
				y = getY(mn, mx, pr, gtop, gbottom);
				mny = getY(mn, mx, prmn, gtop, gbottom);
				mxy = getY(mn, mx, prmx, gtop, gbottom);
				if (bpr != 0) {
					by = getY(mn ,mx, bpr, gtop, gbottom);
					drawLine(hMemDC, x-1, by, x, y, PS_SOLID, RGB(0, 0, 0));
				}
				drawLine(hMemDC, x, mny, x, mxy, PS_SOLID, RGB(0, 0, 0));
				SetPixel(hMemDC, x, y, RGB(0,0,0));
			}
			bpr = pr;
			bidx = idx;

		}

		SelectObject(hMemDC, OldBit);
		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hdc);
	}
}
void DrawDaily::redrawBitmap() {
	if (!loaded) {
		loaded = true;
		CreateThread(NULL, 0, ThreadLoading, this, 0, NULL);
	}
	char buf[4096];
	HWND hWnd = window->hWnd;
	HDC hdc = GetDC(hWnd), hMemDC, hSBitDC;
	HBITMAP OldBit, sOldBit;
	RECT area, rect;
	area = window->getDrawsArea();
	rect.left = 0; rect.right = area.right - area.left;
	rect.top = 0; rect.bottom = area.bottom - area.top;

	if (hBitmap != NULL && memcmp(&hBitRect, &rect, sizeof(RECT))) {
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	if (hBitmap == NULL) {
		hBitmap=CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		hBitRect = rect;
	}
	hMemDC=CreateCompatibleDC(hdc);
	OldBit=(HBITMAP)SelectObject(hMemDC,hBitmap);
	FillRect(hMemDC,&rect,GetSysColorBrush(COLOR_WINDOW));
	Rectangle(hMemDC, rect.left, rect.top, rect.right, rect.bottom);
	switch(status) {
	case DrawDaily::Status::LOADING:
		sprintf_s(buf, sizeof(buf), "Loading ... %.2lf %%", LoadingPhase);
		DrawText(hMemDC, buf, (int)strlen(buf), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	case DrawDaily::Status::LOAD_FAIL:
		sprintf_s(buf, sizeof(buf), "Load Fail");
		DrawText(hMemDC, buf, (int)strlen(buf), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	case DrawDaily::Status::SHOW:
		refreshSBitmap(hBitRect);
		hSBitDC = CreateCompatibleDC(hdc);
		sOldBit = (HBITMAP)SelectObject(hSBitDC, sBitmap);
		BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hSBitDC, 0, 0, SRCCOPY);
		SelectObject(hSBitDC, sOldBit);
		DeleteDC(hSBitDC);
		break;
	}

	SelectObject(hMemDC, OldBit);
	ReleaseDC(hWnd, hdc);
}

HBITMAP DrawDaily::getBitmap() {
	return hBitmap;
}

string DrawDaily::getName() {
	return "시세";
}