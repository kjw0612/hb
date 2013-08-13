// TimeOffsetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OrderBookVI.h"
#include "TimeOffsetDlg.h"
#include "afxdialogex.h"


// TimeOffsetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(TimeOffsetDlg, CDialogEx)

TimeOffsetDlg::TimeOffsetDlg(CWnd* pParent, CObList *doc_list, COrderBookVIDoc *doc)
	: CDialogEx(TimeOffsetDlg::IDD, pParent)
	, doc_list(new CObList())
	, mydoc(doc)
{
	for (POSITION pos = doc_list->GetHeadPosition(); pos != NULL; doc_list->GetNext(pos)) {
		this->doc_list->AddHead( doc_list->GetAt(pos) );
	}
}

TimeOffsetDlg::~TimeOffsetDlg()
{
}

void TimeOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOBOXEX1, Cbox1);
	DDX_Control(pDX, IDC_EDIT1, DAY);
	DDX_Control(pDX, IDC_EDIT2, HOUR);
	DDX_Control(pDX, IDC_EDIT3, MIN);
	DDX_Control(pDX, IDC_EDIT4, SEC);
	DDX_Control(pDX, IDC_EDIT5, MSEC);
}


BEGIN_MESSAGE_MAP(TimeOffsetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &TimeOffsetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// TimeOffsetDlg 메시지 처리기입니다.


void TimeOffsetDlg::OnBnClickedOk()
{
	int select = Cbox1.GetCurSel();
	int i = 0;
	for (POSITION pos = doc_list->GetHeadPosition(); pos != NULL; doc_list->GetNext(pos)) {
		if (i == select) {
			COrderBookVIDoc * doc = (COrderBookVIDoc *)doc_list->GetAt(pos);
			mydoc->isTimeOffset = true;
			mydoc->TimeOffsetFrom = doc;

			CString cday, chour ,cmin ,csec, cmsec;
			DAY.GetWindowTextW(cday);
			HOUR.GetWindowTextW(chour);
			MIN.GetWindowTextW(cmin);
			SEC.GetWindowTextW(csec);
			MSEC.GetWindowTextW(cmsec);
			LL day, hour, min ,sec, msec;
			day = hour = min = sec = msec = 0;
			day = _ttoi(cday);
			hour = _ttoi(chour);
			min = _ttoi(cmin);
			sec = _ttoi(csec);
			msec = _ttoi(cmsec);
			LL diff = day;
			diff = diff * 24 + hour;
			diff = diff * 60 + min;
			diff = diff * 60 + sec;
			diff = diff * 1000000 + msec;
			diff = diff * 1000;
			mydoc->TimeOffsetDiff = diff;
		}
		i ++;
	}
	CDialogEx::OnOK();
}


BOOL TimeOffsetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int i = 0;
	for (POSITION pos = doc_list->GetHeadPosition(); pos != NULL; doc_list->GetNext(pos)) {
		COMBOBOXEXITEMW *item = new COMBOBOXEXITEMW();
		ZeroMemory(item, sizeof(COMBOBOXEXITEMW));

		item->mask = CBEIF_TEXT;
		COrderBookVIDoc * doc = (COrderBookVIDoc *)doc_list->GetAt(pos);
		item->iItem = i;
		item->iImage = i;
		item->iSelectedImage = i;

		int len = lstrlen(doc->lpszPathName);
		item->pszText = (LPWSTR)malloc((len + 1) * sizeof(wchar_t));
		lstrcpy(item->pszText, doc->lpszPathName);

		Cbox1.InsertItem(item);
		i ++;
	}
	Cbox1.SetCurSel(0);

	return TRUE; 
}
