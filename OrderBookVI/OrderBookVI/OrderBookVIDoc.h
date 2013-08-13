
// OrderBookVIDoc.h : COrderBookVIDoc Ŭ������ �������̽�
//


#pragma once
#include "afx.h"

#include "MarketData.h"
#include "STL.h"
#include "iTools.h"
#include "xTime.h"

enum PaintPhase {
	PHASE_INIT,
	PHASE_LOADING,
	PHASE_SIMULATE,
	PHASE_END,
};
class COrderBookVIDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	COrderBookVIDoc();
	DECLARE_DYNCREATE(COrderBookVIDoc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~COrderBookVIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	LPWSTR lpszPathName;
	PaintPhase phase;

	LPWSTR message;
	DWORD dwLoadingNow, dwLoadingAll;
	UINT ThreadLoading(LPVOID pParam);

	vector<MarketDataLine> mdls;
	long long stime;
	long long etime;
	long long ntime;

	map<LL, map<int, pair<int, int> > > mask;
	map<LL, map<int, pair<int, int> > > mbid;
	map<LL, int> mpos, mmoney;
	CWinThread *LoadingThread;
	bool isClosing;
	virtual void OnCloseDocument();
	afx_msg void OnTimeOffset();
	bool isTimeOffset;
	COrderBookVIDoc *TimeOffsetFrom;
	LL TimeOffsetDiff;
};
