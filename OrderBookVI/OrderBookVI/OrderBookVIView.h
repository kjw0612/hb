
// OrderBookVIView.h : COrderBookVIView Ŭ������ �������̽�
//

#pragma once
#include "afxwin.h"

#include "MarketData.h"
#include "STL.h"
#include "iTools.h"
#include "xTime.h"

class COrderBookVIView : public CView
{
protected: // serialization������ ��������ϴ�.
	COrderBookVIView();
	DECLARE_DYNCREATE(COrderBookVIView)

// Ư���Դϴ�.
public:
	COrderBookVIDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~COrderBookVIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	const UINT_PTR idDrawTimer;
public:
	void DrawBuffer(void);
	CBitmap m_Bitmap;
//	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void onInit(void);
	void onLoading(void);
	void onSimulate(void);
	DWORD befTime;
	int playSpeed;
	DWORD view_type;
	DWORD playStop;

	afx_msg void OnStop();
	afx_msg void OnTab();

	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnUp();
	afx_msg void OnDown();

	afx_msg void OnSleft();
	afx_msg void OnSright();
	afx_msg void OnSup();
	afx_msg void OnSdown();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // OrderBookVIView.cpp�� ����� ����
inline COrderBookVIDoc* COrderBookVIView::GetDocument() const
   { return reinterpret_cast<COrderBookVIDoc*>(m_pDocument); }
#endif

