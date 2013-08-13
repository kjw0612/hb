#ifndef __ANALYSE__MARKETDATA__H__
#define __ANALYSE__MARKETDATA__H__
#include "stdafx.h"

#include <memory.h>
#include <stdio.h>
#include "STL.h"
#include "xTime.h"
#include "iTools.h"

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

class MarketDataLine {
public:
	MarketDataLine() {
		memset(this, 0, sizeof(MarketDataLine));
	}
	MarketDataLine(string S) {
		vector<string> token;
		string P = "";
		for (int i=0;i<(int)S.length(); i++ ) {
			if (S[i] == ' ') continue;
			if (S[i] == ',') {
				token.push_back(P);
				P = "";
			} else P += S[i];
		}
		if (P.length() > 0) token.push_back(P);
		assert(token.size() == 41 || token.size() == 42 || token.size() == 43);
		seq = stoi(token[0]);
		type = token[1];
		arriveTime = xTime(token[2]);
		if (token[3].size() < 8) token[3] = "0" + token[3];
		exchangeTime = xTime(token[2].substr(0, 8) + token[3]);
		trade = Trade(stoi(token[4]),
			stoi(token[5]),
			stoi(token[6]));
		
		bid = PQBook( stoi(token[7]), stoi(token[8]));
		for (int i=0;i<5;i++) {
			bid.addPQC( stoi(token[9+3*i]), stoi(token[10+3*i]), stoi(token[11+3*i]) );
		}
		
		ask = PQBook( stoi(token[24]), stoi(token[25]));
		for (int i=0;i<5;i++) {
			ask.addPQC( stoi(token[26+3*i]), stoi(token[27+3*i]), stoi(token[28+3*i]) );
		}
	}
	int seq;
	string type;
	xTime arriveTime, exchangeTime;

	Trade trade;
	PQBook bid, ask;
};

class MarketData {
public:
	MarketData() {}
	MarketData(LPSTR);
	bool nextLine(MarketDataLine&);
	virtual ~MarketData();
public:
//	HGLOBAL *hglobal;
	LPSTR lpText;
	int pos;
};

#endif//__ANALYSE__MARKETDATA__H__