#pragma once

#include "OrderBookVIDoc.h"
#include "afxcmn.h"
#include "afxwin.h"
// TimeOffsetDlg 대화 상자입니다.

class TimeOffsetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TimeOffsetDlg)

public:
	TimeOffsetDlg(CWnd* pParent = NULL, CObList *dlist = NULL, COrderBookVIDoc *doc = NULL);   // 표준 생성자입니다.
	virtual ~TimeOffsetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TIMEOFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBoxEx Cbox1;
	CObList *doc_list;
	virtual BOOL OnInitDialog();
	COrderBookVIDoc* mydoc;
	CEdit DAY;
	CEdit HOUR;
	CEdit MIN;
	CEdit SEC;
	CEdit MSEC;
};
