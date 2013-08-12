// VisTro.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "VisTro.h"
#include "Config.h"
#include "Logger.h"
#include "VisInfo.h"
#include "VisWindow.h"

#include <algorithm>
#include <vector>
#include <string>
#include <map>

#include <commctrl.h>

using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
TCHAR szTabContentsClass[] = "TabContents";

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
ATOM				TabContentsRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	TabProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VisInfo *visInfo;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	Config::Initialize();
	Logger::Initialize();
	visInfo = VisInfo::getInstance();
	visInfo->setDataPath( Config::Get("DataPath") );

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VISTRO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	TabContentsRegisterClass(hInstance);
	VisRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VISTRO));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VISTRO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VISTRO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


ATOM TabContentsRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= TabProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VISTRO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)NULL;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VISTRO);
	wcex.lpszClassName	= szTabContentsClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
HWND g_hWnd = NULL;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 700, 400, NULL, NULL, hInstance, NULL);
   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//


HWND hDayListBox = NULL;
const int idDayListBox = 25001;
HWND hTabControl = NULL;
const int idTabControl = 25002;

void refreshDayListBox() {
	SendMessage(hDayListBox, LB_RESETCONTENT, 0, 0);
	vector<string>& dayList = visInfo->pathInfo.dayList;
	for (int i=0;i<(int)dayList.size();i++) {
		SendMessage(hDayListBox, LB_ADDSTRING, 0, (LPARAM)dayList[i].c_str());
	}
}

string selectedDay = "";
const int TAB_FUTURE = 0;
const int TAB_OPTION = 1;
HWND hFuture = NULL, hOption = NULL;
const int idFuture = 25003;
const int idOption = 25004;

map<int, pair<HWND, string> > tabContents;
int nowTabId = 0;

string curSelectedDay = "";

void DestroyTabContents() {
	for (map<int, pair<HWND, string> >::iterator i = tabContents.begin();
		i != tabContents.end(); i ++) {
		DestroyWindow(i->second.first);
	}
	tabContents.clear();

	if (hFuture != NULL){
		DestroyWindow(hFuture);
		hFuture = NULL;
	}
	if (hOption != NULL){
		DestroyWindow(hOption);
		hOption = NULL;
	}
}

const int idTabContents = 26001;
void addStatic(HWND hWnd, string display, string value,int left,int top,int width,int height) {
	int newId = idTabContents + (int)tabContents.size();
	HWND hStatic = CreateWindow("STATIC", display.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
			left, top, width, height, hWnd, (HMENU)newId, hInst, NULL);
	tabContents[ newId ] = make_pair(hStatic, value);
}
void addButton(HWND hWnd, string display, string value,int left,int top,int width,int height) {
	int newId = idTabContents + (int)tabContents.size();
	HWND hButton = CreateWindow("BUTTON", display.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
			left, top, width, height, hWnd, (HMENU)newId, hInst, NULL);
	tabContents[ newId ] = make_pair(hButton, value);
}

int codeToMonth(char monthC) {
	int month = 0;

	if ('1' <= monthC && monthC <= '9') month = monthC - '0';
	else if ('A' <= monthC && monthC <= 'C') month = monthC - 'A' + 10;

	return month;
}

set<string> getPrefixSet(set<string> S, string prefix) {
	set<string> res;
	for (set<string>::iterator i = S.lower_bound(prefix);
		i != S.end(); i++) {
		if (strncmp(i->c_str(), prefix.c_str(), prefix.length())) break;
		res.insert(*i);
	}
	return res;
}

vector<int> setToVector(set<int> S) {
	vector<int> res;
	for (set<int>::iterator i = S.begin(); i != S.end(); i++) {
		res.push_back(*i);
	}
	return res;
}

string _getCSVSize(string day, string file) {
	char buf[256];
	string filepath = Config::Get("DataPath") + "\\" + day + "\\" + file;

	FILE *fp;
	fopen_s(&fp ,filepath.c_str(), "r");
	fseek(fp, 0L, SEEK_END);
	int szFile = ftell(fp);
	fclose(fp);
	if (szFile < 1000) {
		sprintf_s( buf, sizeof(buf), "%d B", szFile);
		return buf;
	}

	double fileSize = (double)(szFile / 1000);

	const char unit[][32] = {"KB", "MB", "GB"};
	for (int i=0;i<3;i++) {
		if (i == 2 || fileSize <= 1000.0) {
			sprintf_s( buf, sizeof(buf), "%.2lf %s", fileSize, unit[i]);
			return buf;
		}
		fileSize /= 1000.0;
	}
	return "?";
}

map<pair<string, string>, string> hashSize;
string getCSVSize(string day, string file) {
	pair<string, string> key = make_pair(day, file);
	if (hashSize.find(key) == hashSize.end()) {
		hashSize[key] = _getCSVSize(day, file);
	}
	return hashSize[ key ];
}
void CreateTabContents() {
	DestroyTabContents();

	if (curSelectedDay.length() == 0) return;

	RECT rect;
	RECT contentRect;

	GetClientRect(hTabControl, &rect);
	TabCtrl_AdjustRect(hTabControl, FALSE, &rect);
	set<string>& fileSet = visInfo->pathInfo.fileSet[curSelectedDay];
	
	//// FUTURE CONTENTS ////
	hFuture = CreateWindow(szTabContentsClass, NULL,
		WS_CHILD | WS_VISIBLE,
							rect.left, rect.top, 
							rect.right-rect.left, rect.bottom - rect.top,
							hTabControl,(HMENU)idFuture,hInst,NULL);
	
	GetClientRect(hFuture, &contentRect);
	int left, top;
	left = top = 20;
	set<string> fFileSet = getPrefixSet(fileSet, "KR4101");
	for (set<string>::iterator i = fFileSet.begin(); i != fFileSet.end(); i++) {
		addButton(hFuture, i->substr(0, 12).c_str(), i->c_str(), left, top, 200, 20);
		top += 25;
	}

	//// OPTION CONTENTS ////
	hOption = CreateWindow(szTabContentsClass, NULL,
		WS_CHILD | WS_VISIBLE,
							rect.left, rect.top, 
							rect.right-rect.left, rect.bottom - rect.top,
							hTabControl,(HMENU)idOption,hInst,NULL);
	GetClientRect(hOption, &contentRect);

	set<int> monthSet;
	set<int> strikeSet;

	vector<int> monthList, strikeList;
	int monthStart = atoi(curSelectedDay.substr(4,2).c_str());
	map<int, map<int, string> > call, put;

	set<string> cFileSet = getPrefixSet(fileSet, "KR4201");
	set<string> pFileSet = getPrefixSet(fileSet, "KR4301");

	const int t_Call = 0, t_Put = 1;
	for (int t=0; t<2; t++) {
		set<string>& FileSet = (t==t_Call)?cFileSet:pFileSet;

		for (set<string>::iterator i = FileSet.begin(); // Call
			i != FileSet.end(); i++) {

			int month = codeToMonth(i->at(7));
			int price = atoi(i->substr(8,3).c_str());
			if (price % 5 != 0) price = price * 10 + 5;
			else price = price * 10;

			int dmonth = (month - monthStart + 12) % 12;
		
			strikeSet.insert(price);
			monthSet.insert(dmonth);
			if (t == t_Call) call[price][dmonth] = *i;
			if (t == t_Put) put[price][dmonth] = *i;
		}
	}
	
	int width = contentRect.right - contentRect.left;
	int height = contentRect.bottom - contentRect.top;

	int Wcnt = (int)monthSet.size() * 2 + 1;
	int Hcnt = (int)strikeSet.size() + 1;
	int unitWidth = width / Wcnt;
	int unitHeight = height / Hcnt;
	
	monthList = setToVector(monthSet);
	strikeList = setToVector(strikeSet);

	char buf[64];
	for (int i=0;i<Hcnt;i++) {
		top = height * i / Hcnt;
		int strike = -1;
		if (i > 0) strike = strikeList[i-1];
		for (int j=0;j<Wcnt;j++) {
			left = width * j / Wcnt;
			int dmonth = -1;
			if (j != monthSet.size()) {
				int diff = (int)monthList.size() - j;
				if (diff < 0) diff = -diff;
				int cur = diff - 1;
				dmonth = monthList[cur];
			}

			if (i == 0) {
				if (dmonth != -1) {
					int month = (monthStart + dmonth + 11) % 12 + 1;
					sprintf_s(buf, "%d월", month);
					addStatic(hOption, buf, "", left, top, unitWidth, unitHeight);
				}
			} else {
				if (strike != -1 && dmonth != -1) {
					map<int, map<int, string> > & option = (j < (int)monthSet.size()) ? call : put;
					if (option.find(strike) != option.end()) {
						map<int, string>& mp = option[strike];
						if (mp.find(dmonth) != mp.end()) {
							string& value = mp[dmonth];
							addButton(hOption, getCSVSize(curSelectedDay, value), value, left, top, unitWidth, unitHeight);
						}
					}
				} else if (strike != -1) {
					sprintf_s(buf, "%d.%d", strike / 10, strike % 10);
					addStatic(hOption, buf, "", left, top, unitWidth, unitHeight);
				}
			}
		}
	}
}
void refreshTabContents() {
	if (curSelectedDay != selectedDay) {
		curSelectedDay = selectedDay;
		CreateTabContents();
	}
	
	ShowWindow(hFuture, SW_HIDE);
	ShowWindow(hOption, SW_HIDE);
	switch(nowTabId) {
	case TAB_FUTURE:
		ShowWindow(hFuture, SW_SHOW);
		break;
	case TAB_OPTION:
		ShowWindow(hOption, SW_SHOW);
		break;
	}
}

void initTabControl() {
	TCITEM tie;
	SendMessage(hTabControl, TCM_DELETEALLITEMS, 0, 0);
	tie.mask = TCIF_TEXT;
    tie.pszText = "선물";
	SendMessage(hTabControl, TCM_INSERTITEM, TAB_FUTURE, 
        (LPARAM) (LPTCITEM) &tie);

	tie.mask = TCIF_TEXT;
    tie.pszText = "옵션";
	SendMessage(hTabControl, TCM_INSERTITEM, TAB_OPTION, 
        (LPARAM) (LPTCITEM) &tie);
}

void refreshDaySelect() {
	char buf[4096];
	int cur = (int)SendMessage(hDayListBox, LB_GETCURSEL, 0, 0);
	SendMessage(hDayListBox, LB_GETTEXT, (WPARAM)cur, (LPARAM)buf);

	selectedDay = buf;
	refreshTabContents();
}


void setTab(int id) {
	nowTabId = id;
	refreshTabContents();
}

void ResizeAll(int width,int height) {
	if (hDayListBox != NULL) {
		MoveWindow(hDayListBox, 0, 0, 90, height, true);
	}
	if (hTabControl != NULL) {
		MoveWindow(hTabControl, 90, 0, width-90, height, true);
	}

	CreateTabContents();
	refreshTabContents();
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	string filename;
	Document *doc;

	switch (message)
	{
	case WM_CREATE:
        hDayListBox = CreateWindowA("LISTBOX", NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_STANDARD | LBS_NOINTEGRALHEIGHT,
                                0, 0, 90, 350,
                                hWnd,
                                (HMENU) idDayListBox,
                                hInst,
                                NULL);

		hTabControl = CreateWindow(WC_TABCONTROL, NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | TCS_TABS | TCS_FIXEDWIDTH,
								90, 0, 590, 340,
								hWnd,
								(HMENU) idTabControl,
								hInst,
								NULL);
		initTabControl();
        refreshDayListBox();
		break;
	case WM_NOTIFY:
		wmId    = LOWORD(wParam);
		switch (wmId)
		{
		case idTabControl:
			switch (((LPNMHDR)lParam) ->code) {
			case TCN_SELCHANGE:
				setTab(TabCtrl_GetCurFocus(hTabControl));
				break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case idDayListBox:
			switch (wmEvent) {
			case LBN_SELCHANGE:
				refreshDaySelect();
				break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		switch (wmEvent)
		{
		case BN_CLICKED:
			if (tabContents.find(wmId) != tabContents.end()) {
				filename = tabContents[wmId].second;
				doc = visInfo->getDocument(curSelectedDay, filename);
				visInfo->createWindow(doc);
			}
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DestroyTabContents();
		DestroyWindow(hDayListBox);
		DestroyWindow(hTabControl);
		hDayListBox = hTabControl = NULL;
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		ResizeAll(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK TabProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	string filename = "";
	Document *doc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmEvent)
		{
		case BN_CLICKED:
			if (tabContents.find(wmId) != tabContents.end()) {
				filename = tabContents[wmId].second;
				doc = visInfo->getDocument(curSelectedDay, filename);
				visInfo->createWindow(doc);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}