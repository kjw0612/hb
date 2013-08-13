#include "stdafx.h"
#include "MarketData.h"
MarketData::MarketData(LPSTR lpt) {
	lpText = lpt;
	pos = 0;
	
	while(*(lpText+(pos++)) != '\n');
}
 bool MarketData::nextLine(MarketDataLine &mdl) {
	string S = "";
	bool cline = false;

	char c;
	while ((c = *(lpText+(pos++))) != '\0') {
		if (c == '\n'){
			cline = true;
			break;
		}
		S += c;
	}

	if (cline) mdl = MarketDataLine(S);
	return cline;
}
 
MarketData::~MarketData() {
}