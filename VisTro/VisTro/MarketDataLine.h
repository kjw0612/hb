#pragma once

#include <string>
#include "xTime.h"


class Trade {
public:
	Trade() {}
	Trade(int Prc,int Vol,int Dir) : price(Prc), vol(Vol), direction(Dir) {}
	int price, vol, direction;
};

class PQC {
public:
	PQC () {}
	PQC (int prc, int qty, int cnt) : Prc(prc), Qty(qty), Cnt(cnt) {}
	int Prc, Qty, Cnt;
};

class PQBook {
public:
	PQBook() : vcnt(0) { memset(V, 0, sizeof(V)); }
	PQBook(int totQ, int totC) : totQty(totQ), totCnt(totC), vcnt(0) { memset(V, 0, sizeof(V)); }
	void addPQC(int p, int q, int c) {
		if (p == 0) return;
		V[vcnt++] = PQC(p, q, c); 
	}
	bool isEmpty() {
		return vcnt == 0;
	}
	void setV(PQBook x) {
		vcnt = x.vcnt;
		for (int i=0;i<vcnt;i++) {
			V[i] = x.V[i];
		}
	}
//private:
	int totQty, totCnt;
	PQC V[5];
	int vcnt;
};

class MarketDataLine
{
public:
	MarketDataLine(std::string S);
	~MarketDataLine(void);
	
	int seq;
	std::string type;
	xTime arriveTime, exchangeTime;

	Trade trade;
	PQBook bid, ask;

	int expectedPrice;
};

