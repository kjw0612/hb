
// OrderBookVI.h : OrderBookVI ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// COrderBookVIApp:
// �� Ŭ������ ������ ���ؼ��� OrderBookVI.cpp�� �����Ͻʽÿ�.
//

class COrderBookVIApp : public CWinAppEx
{
public:
	COrderBookVIApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	void GetDocumentList(CObList * pDocList);
};

extern COrderBookVIApp theApp;
