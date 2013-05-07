#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "readers.h"

void sample(){
  KospiOptionsReader rdr("data/C161_15515");
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
	while(rdr.next()){
		KrxOptionsHeader optheader;
		unsigned char * timestamp;
		optheader.m_rawmsg = rdr.content;
		switch(rdr.optionsType){
			case t_KrxOptionsTradeBestQuotation:
				timestamp = optheader.m_KrxOptionsTradeBestQuotation->timestamp;
				break;
			case t_KrxOptionsTrade:
				timestamp = optheader.m_KrxOptionsTrade->timestamp;
				break;
			case t_KrxOptionsBestQuotation:
				timestamp = optheader.m_KrxOptionsBestQuotation->timestamp;
				break;
			default:
				continue;
		}
		std::cout << timestamp << std::endl;
	}
}

int main(){
	sample();
	return 0;
}
