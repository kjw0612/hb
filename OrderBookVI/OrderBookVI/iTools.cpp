#include "stdafx.h"
#include "iTools.h"

int stoi(string &S) {
	int res = 0;
	int mi = 0;
	if (S.size() > 0 && S[0] == '-') mi = 1;
	for(int i=mi;i<SZ(S);i++) res = res * 10 + (S[i]-'0');
	if (mi == 1) res = -res;
	return res;
}
LL stoi64(string &S) {
	LL res = 0;
	int mi = 0;
	if (S.size() > 0 && S[0] == '-') mi = 1;
	for(int i=mi;i<SZ(S);i++) res = res * 10 + (S[i]-'0');
	if (mi == 1) res = -res;
	return res;
}
