
// OrderBookVIDoc.h : COrderBookVIDoc 클래스의 인터페이스
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
protected: // serialization에서만 만들어집니다.
	COrderBookVIDoc();
	DECLARE_DYNCREATE(COrderBookVIDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~COrderBookVIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
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
