#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "readers.h"
#include "indexer.h"


DataWindow dw;
DescSet ds;
std::string basepath = "";

void makedesc(){
	//KospiFuturesReader futrdr("data/F191_15571");
	//KospiOptionsReader optrdr("data/C195_15511");
	MakeDesc fmd(basepath+"data\\F191_15571",'f');
	MakeDesc omd(basepath+"data\\C195_15511",'o');
	ds.append(fmd.getDS());
	ds.append(omd.getDS());
}

void setup(){
	std::ifstream fi("synconfig.txt");
	std::string a,b;
	while(!fi.eof()){
		fi >> a;
		std::getline(fi,b);
		while(b[0]==32){
			b = b.substr(1);
		}
		if (!_strcmpi(a.c_str(),"basepath")){
			basepath = b;
		}
	}
}

void indexing(){
	/*
	Indexer indexer(basepath+"data\\C161_15515",'c');
	indexer.run();
	indexer.setIndexTree();
	//indexer.getQuestionTest();
	BlockReader<KospiOptionsReader> kobr(basepath+"data\\C161_15515",&indexer);
	std::vector<Brick *> bricks = kobr.readBlockTime(9000000,9200000);
	*/
	ReaderSet crdst(basepath+"data\\C161_15515",'c');
	std::vector<Brick *> cbricks = crdst.blrd.readBlockTime(9000000,9200000);
	ReaderSet prdst(basepath+"data\\P162_15516",'p');
	std::vector<Brick *> pbricks = prdst.blrd.readBlockTime(9000000,9200000);
	ReaderSet frdst(basepath+"data\\F171_15572",'f');
	std::vector<Brick *> fbricks = frdst.blrd.readBlockTime(9000000,9200000);
}

void sample(int fromtime = 9000000, int totime = 15000000){
	KospiOptionsReader crdr(basepath+"data\\C161_15515");
	KospiOptionsReader prdr(basepath+"data\\P162_15516");
	KospiFuturesReader frdr(basepath+"data\\F171_15572");
	//t_KrxOptionsBestQuotation = 1629858793,// JSHash(B6034,5) 
	//t_KrxOptionsTrade = 1611359895,// JSHash(A3034,5) 
	//t_KrxOptionsTradeBestQuotation = 1618535850,// JSHash(G7034,5) 
	//t_KrxOptionsEnd = 1612413213,// JSHash(A6034,5) 
	//t_KrxOptionsMarketManage = 1611938576,// JSHash(A7034,5) 
	//t_KrxOptionsSpace = 1618603596,// JSHash(G9034,5) 
	//t_KrxOptionsDistribute = 1696059202,// JSHash(O6034,5) 
	//t_KrxOptionsInvestorData = 1701177067,// JSHash(H1034,5) 
	//t_KrxOptionsUnsettled = 1701402316,// JSHash(H2034,5) 
	//t_KrxOptionsMargin = 1702452665,// JSHash(H3034,5) 
	//t_KrxOptionsMarketpriceRecovery = 1612008420,// JSHash(B2034,5) 
	//t_KrxOptionsPolling = 1637815616,// JSHash(I2034,5) 
	//t_KrxOptionsSettleBeforeOpen = 1623615942,// JSHash(M7034,5) 
	//t_KrxOptionsOpenMarketManage = 1623649775,// JSHash(M4034,5) 
	//t_KrxOptionsGreek = 1625671968,// JSHash(N7034,5) 
	//t_KrxOptionsImpvol = 1045394243,// JSHash(,5) 
	int cnt = 0;
	int timestampi = 0;
	double ask1price = 0, currentprice = 0;
	while(1){
		int mintime = std::min(std::min(crdr.timestamp, prdr.timestamp), frdr.timestamp);
		long long optionFutureType = 0;
		KrxOptionsHeader optheader;
		KrxFuturesHeader futheader;
		enum OnlyForWatch { CallOption, PutOption, Future } cpfType;
		int *tti;

		if (mintime==crdr.timestamp){
			crdr.next();
			optionFutureType = crdr.optionsType;
			optheader.m_rawmsg = crdr.content;
			tti = &crdr.timestamp;
			cpfType = CallOption;
		}
		else if (mintime==prdr.timestamp){
			prdr.next();
			optionFutureType = prdr.optionsType;
			optheader.m_rawmsg = prdr.content;
			tti = &prdr.timestamp;
			cpfType = PutOption;
		}
		else{
			frdr.next();
			optionFutureType = frdr.futuresType;
			futheader.m_rawmsg = frdr.content;
			tti = &frdr.timestamp;
			cpfType = Future;
		}

		char timestamp[20] = "", krcode[20] = "";
		switch(optionFutureType){
			case t_KrxFuturesTradeBestQuotation:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesTradeBestQuotation->timestamp);
				ask1price = ATOI_LEN(futheader.m_KrxFuturesTradeBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesTradeBestQuotation->krcode);
				break;
			case t_KrxOptionsTradeBestQuotation:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsTradeBestQuotation->timestamp);
				ask1price = ATOI_LEN(optheader.m_KrxOptionsTradeBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsTradeBestQuotation->krcode);
				break;
			case t_KrxFuturesTrade:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesTrade->timestamp);
				currentprice = ATOI_LEN(futheader.m_KrxFuturesTrade->currentprice) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesTrade->krcode);
				break;
			case t_KrxOptionsTrade:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsTrade->timestamp);
				currentprice = ATOI_LEN(optheader.m_KrxOptionsTrade->currentprice) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsTrade->krcode);
				break;
			case t_KrxFuturesBestQuotation:
				timestampi = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->timestamp);
				ask1price = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, futheader.m_KrxFuturesBestQuotation->krcode);
				break;
			case t_KrxOptionsBestQuotation:
				timestampi = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->timestamp);
				ask1price = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->ask1price) / 100.0;
				COPY_STR(krcode, optheader.m_KrxOptionsBestQuotation->krcode);
				break;
			default:
				continue;
		}
		if (fromtime <= timestampi && timestampi <= totime){
			if (optionFutureType==t_KrxOptionsTrade || optionFutureType==t_KrxFuturesTrade){
				dw.push(krcode, currentprice, timestampi, 1);
			}
			else{
				dw.push(krcode, ask1price, timestampi, 0);
			}
			*tti = timestampi;
			
			if ((++cnt % 10000) == 0){
				printf("%s   %d    %.2lf    %.2lf\n",krcode, timestampi, ask1price, currentprice );
			}
		}
		else if (totime < timestampi){
			return;
		}
	}
}

int main(){
	setup();
	makedesc();
	indexing();
	//sample();
	return 0;
}