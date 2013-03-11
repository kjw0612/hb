//============================================================================
// Name        : network.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include "packetheaders/krx_futures_info.hpp"
#include "packetheaders/krx_futures_factory.hpp"
using namespace std;

char msgbuf[2001] = {0,};

void fGetData(char *p, int len, FILE *fp ){
	while (len -->0 && !feof(fp) ){
		*p++ = fgetc(fp);
	}
}

void headerFactoryTest(){
	FILE *fp = fopen("data/20130306","rb");
	long long rt;
	int sz;
	int nOfHeaders = 50000;
	for (int i=0;i<nOfHeaders;++i){
		fGetData((char *)&rt, 8, fp);
		fGetData((char *)&sz, 4, fp);
		memset(msgbuf,0,sizeof(msgbuf));
		fGetData(msgbuf, sz, fp);
		MakeKrxFuturesHeader headerSoft(msgbuf, false);
		MakeKrxFuturesHeader headerDeep(msgbuf, true);
		if (headerSoft.type == MakeKrxFuturesHeader::t_NoType){

		}
		else{
			printf("%lld %d :",rt,sz);
			if (headerSoft.type == MakeKrxFuturesHeader::t_KrxFuturesDesc){
				printf("KrxFuturesDesc\n");
			}
			else if (headerSoft.type == MakeKrxFuturesHeader::t_KrxFuturesTrade){
				printf("KrxFuturesTrade\n");
			}
			printf("\n");
		}
	}
	printf("%s",msgbuf);
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	//headers_size_check();
	headerFactoryTest();
	return 0;
}
