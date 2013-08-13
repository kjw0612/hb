#pragma once

#include "OrderBookVIDoc.h"
#include "afxcmn.h"
#include "afxwin.h"
// TimeOffsetDlg ��ȭ �����Դϴ�.

class TimeOffsetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TimeOffsetDlg)

public:
	TimeOffsetDlg(CWnd* pParent = NULL, CObList *dlist = NULL, COrderBookVIDoc *doc = NULL);   // ǥ�� �������Դϴ�.
	virtual ~TimeOffsetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TIMEOFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
