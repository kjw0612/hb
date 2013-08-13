
// OrderBookVIDoc.cpp : COrderBookVIDoc Ŭ������ ����
//
#include <afxwin.h>
#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "OrderBookVI.h"
#endif

#include "OrderBookVIDoc.h"
#include "MainFrm.h"
#include "TimeOffsetDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COrderBookVIDoc

IMPLEMENT_DYNCREATE(COrderBookVIDoc, CDocument)

BEGIN_MESSAGE_MAP(COrderBookVIDoc, CDocument)
	ON_COMMAND(ID_TIME_OFFSET, &COrderBookVIDoc::OnTimeOffset)
END_MESSAGE_MAP()


// COrderBookVIDoc ����/�Ҹ�

COrderBookVIDoc::COrderBookVIDoc()
	: LoadingThread(NULL)
	, isTimeOffset(false)
	, TimeOffsetFrom(NULL)
	, TimeOffsetDiff(0)
{
	message = (LPWSTR)malloc(BUFSIZE * sizeof(wchar_t));
	memset(message, 0, BUFSIZE * sizeof(wchar_t));
	lpszPathName = NULL;
	isClosing = false;
}

COrderBookVIDoc::~COrderBookVIDoc()
{
	if (lpszPathName != NULL) free(lpszPathName);
	free(message);
}

BOOL COrderBookVIDoc::OnNewDocument()
{
	return FALSE;

	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// COrderBookVIDoc serialization

void COrderBookVIDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void COrderBookVIDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �˻� ó���⸦ �����մϴ�.
void COrderBookVIDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void COrderBookVIDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// COrderBookVIDoc ����

#ifdef _DEBUG
void COrderBookVIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COrderBookVIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// COrderBookVIDoc ���

UINT ThreadLoadingCaller(LPVOID pParam) {
	return ((COrderBookVIDoc *)pParam)->ThreadLoading(pParam);
}

BOOL COrderBookVIDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	size_t size = lstrlen(lpszPathName);
	this->lpszPathName = (LPWSTR)malloc((size+1) * sizeof(wchar_t));
	lstrcpy(this->lpszPathName, lpszPathName);
	LoadingThread = AfxBeginThread(ThreadLoadingCaller, this);

	return TRUE;
}


UINT COrderBookVIDoc::ThreadLoading(LPVOID pParam)
{
	CFile file;
	file.Open(lpszPathName, CFile::modeRead);
	DWORD dwFileSize = (DWORD)file.GetLength();

	wsprintf(message, _T("Loading %s..."), lpszPathName);
	dwLoadingNow = 0;
	dwLoadingAll = dwFileSize;
	phase = PHASE_LOADING;

	HGLOBAL m_hMem = GlobalAlloc(GHND, dwFileSize);

	LPSTR lpText = (LPSTR)GlobalLock(m_hMem);

	file.Read(lpText, dwFileSize);
	file.Close();

	int len;
	for (len=0; lpszPathName[len] != '\0'; len ++) ;
	
	bool isMarketData = 
		(lpszPathName[ len - 4 ] == '.' &&
		lpszPathName[ len - 3 ] == 'c' &&
		lpszPathName[ len - 2 ] == 's' &&
		lpszPathName[ len - 1 ] == 'v');

	if (isMarketData) {
		MarketData md(lpText);
		MarketDataLine mdl, bmdl;

		mdls = vector<MarketDataLine>();
		while (md.nextLine(mdl)) {
			dwLoadingNow = md.pos;
	//		if (mdl.arriveTime.hour < 9) continue;
	
			if (mdl.trade.direction != 0 &&
				mdl.ask.isEmpty() &&
				mdl.bid.isEmpty()) {

				mdl.ask.setV(bmdl.ask);
				mdl.bid.setV(bmdl.bid);

				PQC *V = (mdl.trade.direction == -1)?mdl.bid.V:mdl.ask.V;
				int vcnt = (mdl.trade.direction == -1)?mdl.bid.vcnt:mdl.ask.vcnt;
				for (int i=0;i<vcnt;i++){
					if(V[i].Prc == mdl.trade.price) {
						V[i].Cnt = -1;
						V[i].Qty -= mdl.trade.vol;
					}
				}
			}
			bmdl = mdl;
			if (!mdl.ask.isEmpty() || !mdl.bid.isEmpty())
				mdls.push_back(mdl);

			if (isClosing) return 0;
		}
		if (!mdls.empty()) {
			stime = mdls[0].arriveTime.getTime();
			etime = mdls[ SZ(mdls) - 1 ].arriveTime.getTime();
		}
		
		ntime = stime;
		dwLoadingNow = md.pos;
	} else {
		int position, money;
		position = money = 0;
		
		map<int, pair<int, int> > ask;
		map<int, pair<int, int> > bid;
		
		const int POS_NEW = 9999;

		char *buf = new char [1024];
		char **P;
		P = new char *[5];
		for (int i=0;i<5;i++) P[i] = new char[128];
		int bp = 0;
		for (int i = 0; i < (int)dwFileSize; i ++) {
			buf[bp ++] = lpText[i];
			if (lpText[i] == '\n') {


				buf[bp] = '\0';

				sscanf_s(buf,"%s %s %s %s %s", P[0], 128, P[1], 128, P[2], 128, P[3], 128, P[4], 128);
				LL now_time;
				sscanf_s(P[0], "%lld", &now_time);

				int onum, prc, dir, pos;
				
				sscanf_s(P[2], "%d", &onum);
				if (!strcmp(P[1], "New")) {
					sscanf_s(P[3], "%d", &prc);
					sscanf_s(P[4], "%d", &dir);
					if (dir == DIR_ASK) {
						ask[ onum ].first = prc;
						ask[ onum ].second = POS_NEW;
					}
					
					if (dir == DIR_BID) {
						bid[ onum ].first = prc;
						bid[ onum ].second = POS_NEW;
					}
				} else if (!strcmp(P[1], "Cancel")) {
					ask.erase(onum);
					bid.erase(onum);

				} else if (!strcmp(P[1], "Move")) {
					sscanf_s(P[3], "%d", &pos);
					if (ask.find(onum) != ask.end()) ask[onum].second = pos;
					if (bid.find(onum) != bid.end()) bid[onum].second = pos;
				} else if (!strcmp(P[1], "Traded")) {
					sscanf_s(P[3], "%d", &prc);
					sscanf_s(P[4], "%d", &dir);
					if (ask.find(onum) != ask.end()){ money += ask[onum].first; position --; }
					if (bid.find(onum) != bid.end()){ money -= bid[onum].first; position ++; }
					ask.erase(onum);
					bid.erase(onum);
				}
				bp = 0;
				mask[now_time] = ask;
				mbid[now_time] = bid;
				mmoney[now_time] = money;
				mpos[now_time] = position;
			}
		}
		for (int i=0;i<5;i++) delete[] P[i];
		delete[] P;
		delete[] buf;
	}
	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);

	phase = PHASE_SIMULATE;
	return 0;
}


void COrderBookVIDoc::OnCloseDocument()
{
	isClosing = true;
	if (phase == PHASE_LOADING) {
		WaitForSingleObject(LoadingThread, INFINITE);
	}

	CDocument::OnCloseDocument();
}


void COrderBookVIDoc::OnTimeOffset()
{
	CObList doc_list;
	((COrderBookVIApp *)AfxGetApp())->GetDocumentList(&doc_list);
	TimeOffsetDlg *dialog = new TimeOffsetDlg(AfxGetApp()->GetMainWnd(), &doc_list, this);

	dialog->Create(IDD_TIMEOFFSET);
	dialog->ShowWindow(SW_SHOW);

}
