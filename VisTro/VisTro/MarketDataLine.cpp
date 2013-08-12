#include "stdafx.h"
#include "MarketDataLine.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

int _stoi(string S) {
	if (S.length() == 0) return 0;
	return stoi(S);
}

MarketDataLine::MarketDataLine(string S)
{
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
	seq = _stoi(token[0]);
	type = token[1];
	arriveTime = xTime(token[2]);
	if (token[3].size() < 8) token[3] = "0" + token[3];
	exchangeTime = xTime(token[2].substr(0, 8) + token[3]);
	trade = Trade(_stoi(token[4]),
		_stoi(token[5]),
		_stoi(token[6]));
		
	bid = PQBook( _stoi(token[7]), _stoi(token[8]));
	for (int i=0;i<5;i++) {
		bid.addPQC( _stoi(token[9+3*i]), _stoi(token[10+3*i]), _stoi(token[11+3*i]) );
	}
		
	ask = PQBook( _stoi(token[24]), _stoi(token[25]));
	for (int i=0;i<5;i++) {
		ask.addPQC( _stoi(token[26+3*i]), _stoi(token[27+3*i]), _stoi(token[28+3*i]) );
	}

	expectedPrice = 0;
	if (token.size() >= 42) {
		expectedPrice = _stoi(token[41]);
	}
}


MarketDataLine::~MarketDataLine(void)
{
}
