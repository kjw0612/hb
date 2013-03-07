/*
 * krx_futures_info.hpp
 *
 *  Created on: 2013. 3. 5.
 *      Author: koval
 */

#ifndef KRX_FUTURES_INFO_HPP_
#define KRX_FUTURES_INFO_HPP_

#include "typedefs.hpp"

struct KrxFuturesDesc{
	/*
	 * 참고문서 : 코스피200지수선물체결호가정보_실시간_회원사_UDP_20121217_2012111901.xls
	 *
	 * SPEC상세 sheet의 K200선물 종목정보 데이터
	 *
	 * no ga da..
	 *
	 */
	byte datatype[2]; // 1 data gubun // A0
	byte infotype[2];
	byte markettype[1];
	byte stocksnum[5];
	byte tradeday[8]; // 5 trade day
	char krcode[12];
	byte seq[6];
	char derivativeid[10];
	char krcodeshort[9];
	char koreanname[80]; // 10 korean security name
	char koreannameshort[40];
	char englishname[80];
	char englishnameshort[40];
	byte issuedate[8];
	byte expirydate[8]; // 15 expiry date on the security
	byte longorshortspread[1];
	byte cashorasset[1];
	byte upperlimitsign[1];
	byte upperlimitprice[12];
	byte lowerlimitsign[1]; // 20 sign on lower limit price
	byte lowerlimitprice[12];
	byte baseprice[12];
	byte underlyingid[3];
	byte americanoreuropean[1];
	byte spreadtype[2]; // 25 spread type (T1, T2, T3, .. T6)
	byte spreadnearbycode[12];
	byte spreaddistantcode[12];
	byte lasttradedate[8];
	byte lastpaydate[8];
	byte monthtype[1]; // 30 1, 2, 3, 4, 5, 6, 7th - distant month futures.
	byte lastexerdate[8];
	byte strikeprice[17];
	byte adjusttype[1]; // c, n, o
	byte tradingunit[17];
	byte tradingmultip[21]; // 35 trading multiplier (ex 999999999999.99999999)
	byte constructiontype[1]; // 0 : n/a , 1 : daily, 2 : historical
	byte issuetype[1]; // 1 : new, 2 : additional, 3 : existing, 4 : first, ...
	byte atmprice[12];
	byte adjustreason[2];
	char underlyingkrcode[12]; // 40 underlying krcode
	byte underlyinglastprice[12];
	byte timetomatdays[7];
	byte adjustbaseprice[17];
	byte basepricetype[2];
	byte dealbasepricetype[1]; // 45 baseprice type for the sake of trading.
	byte lastadjpricesign[1];
	byte lastadjprice[17];
	byte blocktradeyn[1];
	byte prevmarginbaseprice[17];
	byte prevmarginbasepricetype[2]; // 50 prevday baseprice type code
	byte settledtheoreticprice[15];
	byte basethereticprice[15];
	byte prevsettledprice[17];
	byte prohibitedtradeyn[1];
	byte circuitbreakersupperlimit[12]; // 55
	byte circuitbreakerslowerlimit[12];
	byte inquirystrikeprice[17];
	byte moneynesstype[1]; // 0:futures, 1 atm 2 itm 3 otm
	byte lastdayyn[1];
	byte exdividendprice[15]; // 60 배당락 후 배당가치.
	byte lastpricesign[1];
	byte lastprice[12]; // 종가
	byte lastpricetype[1]; // 1 : 실세 2 : 기세(삭제됨) 3 : 거래무
	byte tradepricesign[1];
	byte tradeprice[12]; // 65 전일시가
	byte highpricesign[1];
	byte highprice[12];
	byte lowpricesign[1];
	byte lowprice[12];
	byte firstcontractedtime[8]; // 70 최초체결일자
	byte lastcontractedtime[8];
	byte lastsettlepricetype[2];
	byte settletheordiffsign[1];
	byte settletheoreticdiff[12];
	byte prevdayunsettledcontractnum[10];// 75 전일미결제약정수량
	byte prevbestasksign[1];
	byte prevbestaskprice[12];
	byte prevbestbidsign[1];
	byte prevbestbidprice[12];
	byte impliedvolatility[10]; // 80  내재변동성
	byte underlyingmaxdeltacode[1];
	byte underlyingmaxdelta[6];
	byte alltimehighestsign[1];
	byte alltimehighestprice[12];
	byte alltimelowestsign[1];	// 85 상장중최저가부호
	byte alltimelowestprice[12];
	byte annualhighestsign[1];
	byte annualhighestprice[12];
	byte annuallowestsign[1];
	byte annuallowestprice[12]; // 90 연중최저가
	byte alltimehighestdate[8];
	byte alltimelowestdate[8];
	byte annualhighestdate[8];
	byte annuallowestdate[8];
	byte annualbasedaysnum[8]; //  95 연간기준일수
	byte monthlytradedaysnum[8];
	byte yearlytradedaysnum[8];
	byte prevdaycontractsnum[16];
	byte prevdayquantity[12];
	byte prevdaytotalamount[22]; // 100 전일거래대금
	byte prevdayblocktradequantity[12];
	byte prevdayblocktradeamount[22];
	byte rate_cd[6];
	byte maxunsettledcontractsnum[8];
	byte affiliatedcommoditygroup[4]; // 105 소속상품군
	byte commodityoffsetrate[5];
	byte limitorderconditioncode[1];
	byte marketorderconditioncode[1];
	byte conditionallimitorderconditioncode[1];
	byte bestlimitorderconditioncode[1]; // 110 최유리지정가호가조건구분코드
	byte subjectedtoefpyn[1];
	byte subjectedtoflexyn[1];
	byte prevdayefpquantity[12];
	byte prevdayefpamount[22];
	byte closureyn[1];
	byte filler[133];
	byte eot[1];
};

struct KrxFuturesHeaderPart{
	byte datatype[2]; //
	byte infotype[2]; // 01
	byte markettype[1]; // 4
	char krcode[12];
	byte seq[2];
};

struct KrxFuturesTradePart{
	byte currentpricesign[1];
	byte currentprice[5];
	byte tradequantity[6];
	byte tradetype[2];

	byte timestamp[8];

	byte closesttradeprice[5];
	byte distanttradeprice[5];

	byte marketpricesign[1];
	byte marketprice[5];

	byte highpricesign[1];
	byte highprice[5];

	byte lowpricesign[1];
	byte lowprice[5];

	byte lastpricesign[1];
	byte lastprice[5];

	byte accumtradequantity[7];
	byte accumtradeamount[12];
};

struct KrxFuturesQuotationPart{
	byte bidtotalquantity[6];

	byte bid1sign[1];
	byte bid1price[5];
	byte bid1quantity[6];

	byte bid2sign[1];
	byte bid2price[5];
	byte bid2quantity[6];


	byte bid3sign[1];
	byte bid3price[5];
	byte bid3quantity[6];


	byte bid4sign[1];
	byte bid4price[5];
	byte bid4quantity[6];


	byte bid5sign[1];
	byte bid5price[5];
	byte bid5quantity[6];

	byte asktotalquantity[6];

	byte ask1sign[1];
	byte ask1price[5];
	byte ask1quantity[6];

	byte ask2sign[1];
	byte ask2price[5];
	byte ask2quantity[6];


	byte ask3sign[1];
	byte ask3price[5];
	byte ask3quantity[6];


	byte ask4sign[1];
	byte ask4price[5];
	byte ask4quantity[6];


	byte ask5sign[1];
	byte ask5price[5];
	byte ask5quantity[6];

	byte bidtotalvalidnum[5];
	byte bid1totalvalidnum[4];
	byte bid2totalvalidnum[4];
	byte bid3totalvalidnum[4];
	byte bid4totalvalidnum[4];
	byte bid5totalvalidnum[4];

	byte asktotalvalidnum[5];
	byte ask1totalvalidnum[4];
	byte ask2totalvalidnum[4];
	byte ask3totalvalidnum[4];
	byte ask4totalvalidnum[4];
	byte ask5totalvalidnum[4];
};

struct KrxFuturesBestQuotation{ // K200선물 우선호가 // B6
	KrxFuturesHeaderPart hp;
	byte marketstatecode[2];
	KrxFuturesQuotationPart qp;
	byte timestamp[8];
	byte expectpricesign[1];
	byte expectprice[5];
	byte eot[1];
};

struct KrxFuturesTrade{ // K200선물 체결 // A3
	KrxFuturesHeaderPart hp;
	KrxFuturesTradePart tp;
	byte eot[1];
};

struct KrxFuturesTradeBestQuotation{ // K200선물 체결 우선호가 // G7
	KrxFuturesHeaderPart hp;
	KrxFuturesTradePart tp;
	byte marketstatecode[2];
	KrxFuturesQuotationPart qp;
	byte eot[1];
};

struct KrxFuturesEnd{ // K200선물 종목마감 // A6
	KrxFuturesHeaderPart hp;
	byte endpricesign[1];
	byte endprice[5];
	byte endpricecode[1];
	byte accumtradequantity[7];
	byte accumtradeamount[12];
	KrxFuturesQuotationPart qp;
	byte eot[1];
};

struct KrxFuturesMarketManage{ // K200선물 장운용
	byte datatype[2]; // A7
	byte infotype[2]; // 01
	byte markettype[1]; // 4
	byte timestamp[8];
	byte marketmanagecode[3];
	byte stockid[10];
	char krcode[12];
	byte seq[2];
	byte marketstatecode[2];
	byte bidaskexistyn[1];
	KrxFuturesQuotationPart qp;
	byte eot[1];
};

struct KrxFuturesSpace{ // K200선물 SPACE
	byte datatype[2]; // G9
	byte infotype[2]; // 01
	byte markettype[1]; // 4
	byte eot[1];
};

typedef KrxFuturesSpace KrxFuturesClose; //datatype = H0 // K200선물 마감



inline void headers_size_check(){
	KrxFuturesDesc krxFuturesDesc;
	printf("%d\n",sizeof(KrxFuturesDesc));
	assert(sizeof(KrxFuturesDesc)==1200);

	KrxFuturesBestQuotation krxFuturesBestQuotation;
	printf("%d\n",sizeof(krxFuturesBestQuotation));
	assert(sizeof(KrxFuturesBestQuotation)==218);

	KrxFuturesTrade krxFuturesTrade;
	printf("%d\n",sizeof(krxFuturesTrade));
	assert(sizeof(KrxFuturesTrade)==95);

	KrxFuturesTradeBestQuotation krxFuturesTradeBestQuotation;
	printf("%d\n",sizeof(krxFuturesTradeBestQuotation));
	assert(sizeof(KrxFuturesTradeBestQuotation)==279);

	KrxFuturesEnd krxFuturesEnd;
	printf("%d\n",sizeof(krxFuturesEnd));
	assert(sizeof(KrxFuturesEnd)==228);

	KrxFuturesMarketManage krxFuturesMarketManage;
	printf("%d\n",sizeof(krxFuturesMarketManage));
	assert(sizeof(KrxFuturesMarketManage)==226);
}

#endif /* KRX_FUTURES_INFO_HPP_ */
