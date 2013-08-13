
// OrderBookVIView.cpp : COrderBookVIView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "OrderBookVI.h"
#endif

#include "OrderBookVIDoc.h"
#include "OrderBookVIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COrderBookVIView

IMPLEMENT_DYNCREATE(COrderBookVIView, CView)

BEGIN_MESSAGE_MAP(COrderBookVIView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// COrderBookVIView 생성/소멸

COrderBookVIView::COrderBookVIView()
	: idDrawTimer(1)
	, playSpeed(1)
	, befTime(0)
	, playStop(1)
	, view_type(0)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

COrderBookVIView::~COrderBookVIView()
{
}

BOOL COrderBookVIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// COrderBookVIView 그리기

void COrderBookVIView::OnDraw(CDC* pDC)
{
	COrderBookVIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC dc;
	CRect rect;
	CBitmap *pOldBmp;

	GetClientRect(&rect);
	dc.CreateCompatibleDC(pDC);
	pOldBmp = dc.SelectObject(&m_Bitmap);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

void COrderBookVIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void COrderBookVIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// COrderBookVIView 진단

#ifdef _DEBUG
void COrderBookVIView::AssertValid() const
{
	CView::AssertValid();
}

void COrderBookVIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COrderBookVIDoc* COrderBookVIView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COrderBookVIDoc)));
	return (COrderBookVIDoc*)m_pDocument;
}
#endif //_DEBUG


// COrderBookVIView 메시지 처리기


void COrderBookVIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == idDrawTimer) {
		DrawBuffer();
	}
	CView::OnTimer(nIDEvent);
}


void COrderBookVIView::DrawBuffer(void)
{
	COrderBookVIDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	switch (pDoc->phase) {

	case PHASE_INIT:
		onInit();
		break;
	case PHASE_LOADING:
		onLoading();
		break;
	case PHASE_SIMULATE:
		onSimulate();
		break;
	}
}


//void COrderBookVIView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
//}


int COrderBookVIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetTimer(idDrawTimer, 1000/60, NULL);

	return 0;
}


void COrderBookVIView::onInit(void)
{
}


void COrderBookVIView::onLoading(void)
{
	COrderBookVIDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CClientDC dc(this);
	CRect rect;

	CDC MemDC;
	CBitmap* pOldBmp;
	GetClientRect(&rect);

	m_Bitmap.DeleteObject();
	m_Bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	MemDC.CreateCompatibleDC(&dc);
	pOldBmp = MemDC.SelectObject(&m_Bitmap);
	MemDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);

	MemDC.DrawText(pDoc->message, CRect(50,0,500,100), DT_LEFT);
	LPWSTR str = (LPWSTR)malloc(BUFSIZE);
	int tt = (int)((double)pDoc->dwLoadingNow * 10000 / pDoc->dwLoadingAll);
	wsprintf(str, _T("%d.%d %%"),tt/100, tt%100);
	MemDC.DrawText(str, CRect(50,100,500,200), DT_LEFT);
	free(str);
	Invalidate (FALSE);

	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
}


void COrderBookVIView::onSimulate(void)
{
	COrderBookVIDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	DWORD difTime, nowTime;
	nowTime = GetTickCount();
	if (befTime == 0) befTime = nowTime;
	difTime = nowTime - befTime;
	befTime = nowTime;

	if (pDoc->isTimeOffset) {
		pDoc->ntime = pDoc->TimeOffsetFrom->ntime + pDoc->TimeOffsetDiff;
	} else {
		pDoc->ntime += (long long)difTime * 1000000ll * playSpeed * playStop;
		if (pDoc->ntime < pDoc->stime) pDoc->ntime = pDoc->stime;
		if (pDoc->ntime > pDoc->etime) pDoc->ntime = pDoc->etime;
	}
	CClientDC dc(this);
	CRect rect;

	CDC MemDC;
	CBitmap* pOldBmp;

	GetClientRect(&rect);

	m_Bitmap.DeleteObject();
	m_Bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	MemDC.CreateCompatibleDC(&dc);
	pOldBmp = MemDC.SelectObject(&m_Bitmap);
	MemDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);

	/*
	TODO : Draw Hoga
	*/
	/*
	MemDC.MoveTo(100, 100);
	MemDC.LineTo(700, 100);
	MemDC.Rectangle(10, 10, 510, 17);
	MemDC.DrawText(_T("123"), CRect(50,0,100,100), DT_RIGHT);
	*/
	CPen pen, *open = NULL;
	CBrush brush, *obrush = NULL;
	int center = (rect.Width()-99 + 54+45-1)/2;

	int s, e, m;
	s = 1; e = SZ(pDoc->mdls)-1;
	while (s<=e) {
		m = (s+e) / 2;
		if (pDoc->ntime < pDoc->mdls[m].arriveTime.getTime()) e = m-1;
		else s = m+1;
	}
	m = s - 1;

	
	int dif = 0;
	CString askp[5], bidp[5];
	CString askq[5], bidq[5];
	CString seq, time, speed;
	CString position, money;
		
	int askv[5] = {0}, bidv[5] = {0};
	int askc[5] = {0}, bidc[5] = {0};

	int pos_ask[5] = {0}, pos_bid[5] = {0};
	int iask[5] = {0}; // issue
	int ibid[5] = {0};

	if (SZ(pDoc->mdls) > 0) {

		MarketDataLine mdl = pDoc->mdls[s - 1];

		MarketDataLine bmdl;
		LL now_time = pDoc->ntime/1000;
		if (s - 2 >= 0) {
			bmdl = pDoc->mdls[s - 2];
		}


		if (mdl.ask.vcnt > 0 && mdl.bid.vcnt > 0) {
			dif = (mdl.ask.V[0].Prc - mdl.bid.V[0].Prc - 5) / 5;
			char nbuf[64]={0};
			for(int i=0;i<mdl.ask.vcnt;i++) {
				map<LL, map<int, pair<int, int> > >::iterator m = pDoc->mask.upper_bound(now_time);
				if (pDoc->mask.begin() != m) {
					m --;
					map<int, pair<int, int> >& ask = m->second;
					for (map<int, pair<int, int> >::iterator m = ask.begin();
						m != ask.end(); m ++) {
						if (m->second.first == mdl.ask.V[i].Prc) {
							pos_ask[4-i] = m->second.second;
						}
					}
				}
				sprintf_s(nbuf, 64-1, "%d", mdl.ask.V[i].Prc);
				askp[4-i] = nbuf;
				askc[4-i] = mdl.ask.V[i].Prc;

				sprintf_s(nbuf, 64-1, "%d", mdl.ask.V[i].Qty);
				askq[4-i] = nbuf;
				
				askv[4-i] = mdl.ask.V[i].Qty;
			}
			
			for(int i=0;i<mdl.bid.vcnt;i++) {
				map<LL, map<int, pair<int, int> > >::iterator m = pDoc->mbid.upper_bound(now_time);
				if (pDoc->mbid.begin() != m) {
					m --;
					map<int, pair<int, int> >& bid = m->second;
					for (map<int, pair<int, int> >::iterator m = bid.begin();
						m != bid.end(); m ++) {
						if (m->second.first == mdl.bid.V[i].Prc) {
							pos_bid[i] = m->second.second;
						}
					}
				}
				sprintf_s(nbuf, 64-1, "%d", mdl.bid.V[i].Prc);
				bidp[i] = nbuf;
				bidc[i] = mdl.bid.V[i].Prc;

				sprintf_s(nbuf, 64-1, "%d", mdl.bid.V[i].Qty);
				bidq[i] = nbuf;

				bidv[i] = mdl.bid.V[i].Qty;
			}

			if (bmdl.ask.vcnt > 0 && bmdl.bid.vcnt) {
				int bpr = bmdl.bid.V[0].Prc, apr = bmdl.ask.V[0].Prc;
				for (int i=0;i<mdl.ask.vcnt;i++) {
					if (bpr <= mdl.ask.V[i].Prc && mdl.ask.V[i].Prc < apr) {
						iask[4-i] = 1;
						continue;
					}
					for (int j=0;j<bmdl.ask.vcnt;j++) {
						if (mdl.ask.V[i].Prc == bmdl.ask.V[j].Prc) {
							iask[4-i] = (mdl.ask.V[i].Qty != bmdl.ask.V[j].Qty);
						}
					}
				}
				
				for (int i=0;i<mdl.bid.vcnt;i++) {
					if (bpr < mdl.bid.V[i].Prc && mdl.bid.V[i].Prc <= apr) {
						ibid[i] = 1;
						continue;
					}
					for (int j=0;j<bmdl.ask.vcnt;j++) {
						if (mdl.bid.V[i].Prc == bmdl.bid.V[j].Prc) {
							ibid[i] = (mdl.bid.V[i].Qty != bmdl.bid.V[j].Qty);
						}
					}
				}
				
				//vector<PQC>&bV = (mdl.trade.direction == -1)?bmdl.bid.V:bmdl.ask.V;
				PQC *V = (mdl.trade.direction == -1)?mdl.bid.V:mdl.ask.V;
				int vcnt = (mdl.trade.direction == -1)?mdl.bid.vcnt:mdl.ask.vcnt;
				for (int i=0;i<vcnt;i++) {
					if (V[i].Prc == mdl.trade.price) {
						if (mdl.trade.direction == -1) ibid[i] = 2;
						else iask[4-i] = 2;
					}
				}
			}
			sprintf_s(nbuf, 64-1, "x %d", playSpeed);
			speed = nbuf;

			sprintf_s(nbuf, 64-1, "seq : %d (x%d)", mdl.seq, playSpeed);
			seq = nbuf;
			
			sprintf_s(nbuf, 64-1, "time : %s", mdl.arriveTime.toView().c_str());
			time = nbuf;

			map<LL, int>::iterator m;
			m = pDoc->mpos.upper_bound( now_time );
			int pos = 0;
			if (m != pDoc->mpos.begin() ) {
				m --;
				sprintf_s(nbuf, 64-1, "position : %d", m->second);
				pos = m->second;
				position = nbuf;
			}

			m = pDoc->mmoney.upper_bound( now_time );
			if (m != pDoc->mmoney.begin() ) {
				m --;
				sprintf_s(nbuf, 64-1, "money : %d / %d", m->second, m->second + pos * (pos>0 ? mdl.bid.V[0].Prc : mdl.ask.V[0].Prc));
				money = nbuf;
			}

		}

	}
	MemDC.DrawText(seq, CRect(5, 0, rect.Width(), 15), DT_LEFT);
	MemDC.DrawText(time, CRect(5, 15, rect.Width(), 30), DT_LEFT);
	MemDC.DrawText(money, CRect(5, 30, rect.Width(), 45), DT_LEFT);
	MemDC.DrawText(position, CRect(5, 45, rect.Width(), 60), DT_LEFT);
	//MemDC.DrawText(speed, CRect(5, 0, rect.Width(), 15), DT_LEFT);
	if (view_type == 0 || view_type == 2 || view_type == 3) {
	
		int dH = 5;
		// Ask Order (Reverse)
		for (int i=0;i<5;i++) {
			if (iask[i] == 2) {
				pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
				open = MemDC.SelectObject(&pen);
			}
			else if (iask[i] == 1) {
				pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
				open = MemDC.SelectObject(&pen);
			}
			MemDC.Rectangle(10, 100+19*i-dif*dH, 55, 120+19*i-dif*dH);
			MemDC.DrawText(askp[i], CRect(10,100+2+19*i-dif*dH,55-2,120+19*i-dif*dH), DT_RIGHT);
			MemDC.Rectangle(54, 100+19*i-dif*dH, 54+45, 120+19*i-dif*dH);
			MemDC.DrawText(askq[i], CRect(54,100+2+19*i-dif*dH,54+45-2,120+19*i-dif*dH), DT_RIGHT);
			if (iask[i] == 1 || iask[i] == 2) {
				MemDC.SelectObject(open);
				pen.DeleteObject();
			}
		}


	
		for (int i=0;i<5;i++) {
			int od = (askc[i]) % 7;
			int ed = (askc[i]) % 2;
			if (iask[i] == 2) pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
			else if (iask[i]) pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
			else pen.CreatePen(PS_SOLID, 1, RGB(255-od*30, ed*100, ed*100));
			brush.CreateSolidBrush(RGB(255-od*30, ed*100, ed*100));
			open = MemDC.SelectObject(&pen);
			obrush = MemDC.SelectObject(&brush);

			MemDC.Rectangle(center-askv[i], 100+19*i-dif*dH+1, center, 120+19*i-dif*dH-1);

			MemDC.SelectObject(open);
			MemDC.SelectObject(obrush);
			pen.DeleteObject();
			brush.DeleteObject();

			if (pos_ask[i] != 0) {
				pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
				open = MemDC.SelectObject(&pen);
				MemDC.MoveTo(center - pos_ask[i],100+19*i-dif*dH+1);
				MemDC.LineTo(center - pos_ask[i],120+19*i-dif*dH-1);
				int cx, cy, r = 4;
				cx = center - pos_ask[i];
				cy = ((100+19*i-dif*dH+1) + (120+19*i-dif*dH-1)) / 2;
				RECT rect;
				rect.top = cy-r;
				rect.left = cx-r;
				rect.bottom = cy+r;
				rect.right = cx+r;
				MemDC.Ellipse(&rect);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}

		}
	
		for (int i=0;i<=dif;i++) {
			MemDC.MoveTo(10,195-dH*dif + 2*dH*i);
			MemDC.LineTo(rect.Width()-10,195-dH*dif + 2*dH*i);
		}

		MemDC.MoveTo(center,10);
		MemDC.LineTo(center,380);
	
		// Bid Order
		for (int i=0;i<5;i++) {
			if (ibid[i] == 2) {
				pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
				open = MemDC.SelectObject(&pen);
			}
			else if (ibid[i] == 1) {
				pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
				open = MemDC.SelectObject(&pen);
			}
			MemDC.Rectangle(rect.Width()-99, 195+19*i+dif*dH, rect.Width()-54, 215+19*i+dif*dH);
			MemDC.DrawText(bidq[i], CRect(rect.Width()-99+3,195+2+19*i+dif*dH,rect.Width()-54-2,215+19*i+dif*dH), DT_LEFT);
			MemDC.Rectangle(rect.Width()-55, 195+19*i+dif*dH, rect.Width()-10, 215+19*i+dif*dH);
			MemDC.DrawText(bidp[i], CRect(rect.Width()-55+3,195+2+19*i+dif*dH,rect.Width()-10-2,215+19*i+dif*dH), DT_LEFT);
		
			if (ibid[i] == 1 || ibid[i] == 2) {
				MemDC.SelectObject(open);
				pen.DeleteObject();
			}
		}

		for (int i=0;i<5;i++) {
			int od = (bidc[i]) % 7;
			int ed = (bidc[i]) % 2;
		
			if (ibid[i] == 2) pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
			else if (ibid[i]) pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
			else pen.CreatePen(PS_SOLID, 1, RGB(ed*100, ed*100, 255-od*30));

			brush.CreateSolidBrush(RGB(ed*100, ed*100, 255-od*30));
			open = MemDC.SelectObject(&pen);
			obrush = MemDC.SelectObject(&brush);

			MemDC.Rectangle(center+1, 195+19*i+dif*dH+1, center+1+bidv[i], 215+19*i+dif*dH-1);

			MemDC.SelectObject(open);
			MemDC.SelectObject(obrush);
			pen.DeleteObject();
			brush.DeleteObject();
			if (pos_bid[i] != 0) {
				pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
				open = MemDC.SelectObject(&pen);
				MemDC.MoveTo(center +1+ pos_bid[i],195+19*i+dif*dH+1);
				MemDC.LineTo(center +1+ pos_bid[i],215+19*i+dif*dH-1);
				int cx, cy, r = 4;
				cx = center +1+ pos_bid[i];
				cy = ((195+19*i+dif*dH+1) + (215+19*i+dif*dH-1)) / 2;
				RECT rect;
				rect.top = cy-r;
				rect.left = cx-r;
				rect.bottom = cy+r;
				rect.right = cx+r;
				MemDC.Ellipse(&rect);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}

		}
	}
	if (view_type == 1 || view_type == 2 || view_type == 3) {
		
		int ply = 0;
		if (view_type == 3) {
			ply = 380;
		}
		MemDC.MoveTo(10,195+ply);
		MemDC.LineTo(rect.Width()-10,195+ply);

		MemDC.MoveTo(center,10+ply);
		MemDC.LineTo(center,380+ply);
		//m-1 -> m = center

		//m-k

		//m+k
		const int K = 100;
		for (int i=m-K;i<=m+K;i++) {
			if (i-1 < 0 || i >= SZ(pDoc->mdls)) continue;

			MarketDataLine bmdl= pDoc->mdls[i-1];
			MarketDataLine& mdl = pDoc->mdls[i];

			int L = 3;

			int x = center + i * (L+2) - m * (L+2);
			int MG = 5;

			
			if (mdl.ask.V[0].Prc != bmdl.ask.V[0].Prc) {
				if (mdl.ask.V[0].Prc > bmdl.ask.V[0].Prc)	pen.CreatePen(PS_SOLID, L+2, RGB(255, 0, 0)); 
				if (mdl.ask.V[0].Prc < bmdl.ask.V[0].Prc)	pen.CreatePen(PS_SOLID, L+2, RGB(0, 0, 255));
				open = MemDC.SelectObject(&pen);

				MemDC.MoveTo(x, 10 + ply);
				MemDC.LineTo(x,195-1 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}
			
			if (mdl.bid.V[0].Prc != bmdl.bid.V[0].Prc) {
				if (mdl.bid.V[0].Prc < bmdl.bid.V[0].Prc)	pen.CreatePen(PS_SOLID, L+2, RGB(255, 0, 0));
				if (mdl.bid.V[0].Prc > bmdl.bid.V[0].Prc)	pen.CreatePen(PS_SOLID, L+2, RGB(0, 0, 255));
				open = MemDC.SelectObject(&pen);

				MemDC.MoveTo(x,195+1 + ply);
				MemDC.LineTo(x,380 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}

			int tqty = 0;
			if (mdl.trade.direction != 0) {
				pen.CreatePen(PS_SOLID, L, RGB(200, 200, 0));
				open = MemDC.SelectObject(&pen);
				 
				MemDC.MoveTo(x, 195 - mdl.trade.direction + ply);
				MemDC.LineTo(x, 195 - (mdl.trade.vol * mdl.trade.direction)*MG - mdl.trade.direction + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
				
				PQC *V = (mdl.trade.direction == -1)?bmdl.bid.V:bmdl.ask.V;
				int vcnt = (mdl.trade.direction == -1)?bmdl.bid.vcnt:bmdl.ask.vcnt;
				for (int i=0;i<vcnt;i++) {
					if (V[i].Prc == mdl.trade.price) {
						V[i].Qty -= mdl.trade.vol;
					}
				}
			}

			int aiqty, arqty, biqty, brqty;
			aiqty = arqty = biqty = brqty = 0;
			if (mdl.ask.V[0].Prc == bmdl.ask.V[0].Prc) {// 같은가격
				int diff = (mdl.ask.V[0].Qty - bmdl.ask.V[0].Qty);
				if (diff > 0) aiqty = diff;
				else arqty = -diff;
			} else if (mdl.ask.V[0].Prc > bmdl.ask.V[0].Prc) {
				arqty = bmdl.ask.V[0].Qty;
			} else if (mdl.ask.V[0].Prc < bmdl.ask.V[0].Prc) {
				aiqty = mdl.ask.V[0].Qty;
			}

			if (mdl.bid.V[0].Prc == bmdl.bid.V[0].Prc) {
				int diff = (mdl.bid.V[0].Qty - bmdl.bid.V[0].Qty);
				if (diff > 0) biqty = diff;
				else brqty = -diff;
			} else if (mdl.bid.V[0].Prc < bmdl.bid.V[0].Prc) {
				brqty = bmdl.bid.V[0].Qty;
			} else if (mdl.bid.V[0].Prc > bmdl.bid.V[0].Prc) {
				biqty = mdl.bid.V[0].Qty;
			}
			
			if (aiqty > 0){
				pen.CreatePen(PS_SOLID, L, RGB(0, 255, 0));
				open = MemDC.SelectObject(&pen);
				 
				MemDC.MoveTo(x, 195 - 1 + ply);
				MemDC.LineTo(x, 195 - (aiqty)*MG - 1 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}
			if (arqty > 0){
				pen.CreatePen(PS_SOLID, L, RGB(0, 0, 0));
				open = MemDC.SelectObject(&pen);
				 
				MemDC.MoveTo(x, 195 - 1 + ply);
				MemDC.LineTo(x, 195 - (arqty)*MG - 1 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}

			
			if (biqty > 0){
				pen.CreatePen(PS_SOLID, L, RGB(0, 255, 0));
				open = MemDC.SelectObject(&pen);
				 
				MemDC.MoveTo(x, 195 + 1 + ply);
				MemDC.LineTo(x, 195 + (biqty)*MG + 1 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}
			if (brqty > 0){
				pen.CreatePen(PS_SOLID, L, RGB(0, 0, 0));
				open = MemDC.SelectObject(&pen);
				 
				MemDC.MoveTo(x, 195 + 1 + ply);
				MemDC.LineTo(x, 195 + (brqty)*MG + 1 + ply);

				MemDC.SelectObject(open);
				pen.DeleteObject();
			}

			if (!mdl.ask.isEmpty() && !mdl.bid.isEmpty() && !bmdl.ask.isEmpty() && !bmdl.bid.isEmpty() && i != m-K) {
				int qty, bqty;
				bqty = 0;
				if (mdl.trade.direction == DIR_ASK) bqty = mdl.trade.vol;
				for (int k = 0; k < 5; k ++ ){
					
					bqty += bmdl.ask.V[k].Qty;
					int od = (bmdl.ask.V[k].Prc / 5) % 5;
					int ed = (bmdl.ask.V[k].Prc / 5) % 2;
					qty = 0;
					for (int j = -1; j < 5; j ++) {
						if( j >= 0 )qty += mdl.ask.V[j].Qty;
						if ( (j < 0 && mdl.ask.V[0].Prc > bmdl.ask.V[k].Prc) ||
							 (j >= 0 && mdl.ask.V[j].Prc == bmdl.ask.V[k].Prc) ) {
							pen.CreatePen(PS_SOLID, 1, RGB(255-od*30, ed*100, ed*100));
							open = MemDC.SelectObject(&pen);
							MemDC.MoveTo(x-(L+2), 195-1-bqty + ply);
							MemDC.LineTo(x,195-1-qty + ply);
							MemDC.SelectObject(open);	 
							pen.DeleteObject();
							break;
						}
					}
				}

				bqty = 0;
				if (mdl.trade.direction == DIR_BID) bqty = mdl.trade.vol;
				for (int k = 0; k < 5; k ++ ){
					
					bqty += bmdl.bid.V[k].Qty;
					int od = (bmdl.bid.V[k].Prc / 5) % 5;
					int ed = (bmdl.bid.V[k].Prc / 5) % 2;
					qty = 0;
					for (int j = -1; j < 5; j ++) {
						if( j >= 0 )qty += mdl.bid.V[j].Qty;
						if ( (j < 0 && mdl.bid.V[0].Prc < bmdl.bid.V[k].Prc) ||
							 (j >= 0 && mdl.bid.V[j].Prc == bmdl.bid.V[k].Prc) ) {
							pen.CreatePen(PS_SOLID, 1, RGB(ed*100, ed*100, 255-od*30));
							open = MemDC.SelectObject(&pen);
							MemDC.MoveTo(x-(L+2), 195+1+bqty + ply);
							MemDC.LineTo(x,195+1+qty + ply);
							MemDC.SelectObject(open);	 
							pen.DeleteObject();
							break;
						}
					}
				}
			}

		}
	
	}
	MemDC.Rectangle(5, rect.Height()-30, 30, rect.Height()-5);
	MemDC.Rectangle(35, rect.Height()-30, 60, rect.Height()-5);
	MemDC.Rectangle(65, rect.Height()-10, rect.Width()-5, rect.Height()-5);
	
	long long now, all;
	int pos;
	all = pDoc->etime - pDoc->stime;
	now = pDoc->ntime - pDoc->stime;
	pos = 66;
	if (all > 0) {
		pos = (int)(now * (rect.Width() - 66) / all) + 66;
	}
	pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
	brush.CreateSolidBrush(RGB(0, 0, 255));
	open = MemDC.SelectObject(&pen);
	obrush = MemDC.SelectObject(&brush);

	MemDC.Rectangle(66, rect.Height()-9, pos+3, rect.Height()-6);
//	MemDC.Rectangle(66, rect.Height()-9, 69, rect.Height()-6);
//	MemDC.Rectangle(rect.Width()-9, rect.Height()-9, rect.Width()-6, rect.Height()-6);
	
	MemDC.SelectObject(open);
	MemDC.SelectObject(obrush);
	pen.DeleteObject();
	brush.DeleteObject();
		
	Invalidate (FALSE);

	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
}

void COrderBookVIView::OnStop()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	playStop = 1-playStop;
}


void COrderBookVIView::OnLeft()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (playSpeed == 1) playSpeed = -1;
	else{
		if (playSpeed < 0) playSpeed *= 2;
		if (playSpeed > 0) playSpeed /= 2;
	}
}


void COrderBookVIView::OnRight()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (playSpeed == -1) playSpeed = 1;
	else{
		if (playSpeed > 0) playSpeed *= 2;
		if (playSpeed < 0) playSpeed /= 2;
	}
}


void COrderBookVIView::OnUp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	COrderBookVIDoc* pDoc = GetDocument();
	int s, e, m;
	s = 1; e = SZ(pDoc->mdls)-1;
	while (s<=e) {
		m = (s+e) / 2;
		if (pDoc->ntime < pDoc->mdls[m].arriveTime.getTime()) e = m-1;
		else s = m+1;
	}
	if (s > 0) s--;

	long long nw = pDoc->mdls[s].arriveTime.getTime();
	do{
		MarketDataLine mdl = pDoc->mdls[s];
		pDoc->ntime = mdl.arriveTime.getTime();
	}while(s-->0 && pDoc->ntime == nw);
}


void COrderBookVIView::OnDown()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	COrderBookVIDoc* pDoc = GetDocument();
	int s, e, m;
	s = 1; e = SZ(pDoc->mdls)-1;
	while (s<=e) {
		m = (s+e) / 2;
		if (pDoc->ntime < pDoc->mdls[m].arriveTime.getTime()) e = m-1;
		else s = m+1;
	}
	if (s > 0) s--;

	long long nw = pDoc->mdls[s].arriveTime.getTime();

	do{
		MarketDataLine mdl = pDoc->mdls[s];
		pDoc->ntime = mdl.arriveTime.getTime();
	}while(++s < SZ(pDoc->mdls) && pDoc->ntime == nw);
}


void COrderBookVIView::OnTab()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	view_type = (view_type + 1)%4;
}




void COrderBookVIView::OnSup()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	COrderBookVIDoc* pDoc = GetDocument();
	LL now_time = pDoc->ntime/1000;
	map<LL, int>::iterator m = pDoc->mpos.upper_bound( now_time );
	
	if (m != pDoc->mpos.begin()) m --;
	if (m != pDoc->mpos.begin()){
		m --;
		pDoc->ntime = m->first * 1000;
	}
}


void COrderBookVIView::OnSdown()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	COrderBookVIDoc* pDoc = GetDocument();
	LL now_time = pDoc->ntime/1000;
	map<LL, int>::iterator m = pDoc->mpos.upper_bound( now_time );
	
	pDoc->ntime = m->first * 1000;
}


void COrderBookVIView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nChar) {
	case VK_DOWN:
		if (GetKeyState (VK_SHIFT) & 0x8000) 
			OnSdown();
		else 
			OnDown();
		break;
	case VK_UP:
		if (GetKeyState (VK_SHIFT) & 0x8000) 
			OnSup();
		else 
			OnUp();
		break;
	case VK_LEFT:
		OnLeft();
		break;
	case VK_RIGHT:
		OnRight();
		break;
	case VK_SPACE:
		OnStop();
		break;
	case VK_TAB:
		OnTab();
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
