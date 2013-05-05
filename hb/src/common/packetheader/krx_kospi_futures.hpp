#ifndef KRX_KOSPI_FUTURES_HPP
#define KRX_KOSPI_FUTURES_HPP

#include "jshash.hpp"

enum eKrxFuturesHeader{
    t_KrxFuturesDesc = 211261866292ll,// A0014
    t_KrxFuturesBestQuotation = 211262259764ll,// B6014
    t_KrxFuturesTrade = 211262062900ll,// A3014
    t_KrxFuturesTradeBestQuotation = 211262326580ll,// G7014
    t_KrxFuturesEnd = 211262259508ll,// A6014
    t_KrxFuturesMarketManage = 211262325044ll,// A7014
    t_KrxFuturesSpace = 211262457652ll,// G9014
    t_KrxFuturesDistribute = 211262263092ll,// O6014
    t_KrxFuturesInvestorData = 211261933620ll,// H1014
    t_KrxFuturesUnsettled = 211261999156ll,// H2014
    t_KrxFuturesSettled = 211262064692ll,// H3014
    t_KrxFuturesMarketpriceRecovery = 211261997620ll,// B2014
    t_KrxFuturesPolling = 211261999412ll,// I2014
    t_KrxFuturesOpenMarketManage = 211262131508ll,// M4014
    t_KrxFuturesClose = 211261868084ll,// H0014

};
inline eKrxFuturesHeader getTypeKrxFuturesHeader(const char *str){
    return (eKrxFuturesHeader) ((((long long)*(int*)str) << 8) + str[4]);
}


struct KrxFuturesDesc{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char stocksnum[5];
	unsigned char tradeday[8];
	char krcode[12];	// 최종 record '999999999999' set
	unsigned char seq[6];
	char derivativeid[10];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	char krcodeshort[9];	// 에) KR4101C90009 → K101C9000
	char koreanname[80];
	char koreannameshort[40];
	char englishname[80];
	char englishnameshort[40];
	unsigned char issuedate[8];
	unsigned char expirydate[8];
	unsigned char longorshortspread[1];	// F:원월종목; N:근월종목
	unsigned char cashorasset[1];	// C:현금결제; D:실물인수도결제
	unsigned char upperlimitsign[1];
	unsigned char upperlimitprice[12];	// 예) 9999999999.99
	unsigned char lowerlimitsign[1];
	unsigned char lowerlimitprice[12];	// 예) 9999999999.99
	unsigned char baseprice[12];	// 예) 9999999999.99
	unsigned char underlyingid[3];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char americanoreuropean[1];	// A:미국형;E:유럽형;Z:기타
	unsigned char spreadtype[2];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char spreadnearbycode[12];
	unsigned char spreaddistantcode[12];
	unsigned char lasttradedate[8];
	unsigned char lastpaydate[8];
	unsigned char monthtype[1];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char lastexerdate[8];	// 권리행사 가능한 최종일자
	unsigned char strikeprice[17];	// 예) 999999999.99999999
	unsigned char adjusttype[1];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char tradingunit[17];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char tradingmultip[21];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char constructiontype[1];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char issuetype[1];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char atmprice[12];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char adjustreason[2];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	char underlyingkrcode[12];
	unsigned char underlyinglastprice[12];	// 예) 9999999999.99
	unsigned char timetomatdays[7];
	unsigned char adjustbaseprice[17];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char basepricetype[2];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char dealbasepricetype[1];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char lastadjpricesign[1];
	unsigned char lastadjprice[17];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char blocktradeyn[1];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char prevmarginbaseprice[17];	// 예) 999999999.99999999
	unsigned char prevmarginbasepricetype[2];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char settledtheoreticprice[15];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char basethereticprice[15];	// 예) 999999999.999999
	unsigned char prevsettledprice[17];	// 예) 999999999.99999999
	unsigned char prohibitedtradeyn[1];	//  Y; N
	unsigned char circuitbreakersupperlimit[12];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 +5%를 계산한 가격) 예) 9999999999.99
	unsigned char circuitbreakerslowerlimit[12];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 -5%를 계산한 가격) 예) 9999999999.99
	unsigned char inquirystrikeprice[17];	// 예) 999999999.99999999
	unsigned char moneynesstype[1];	// 0:선물; 1:ATM; 2:ITM; 3:OTM
	unsigned char lastdayyn[1];	// Y; N
	unsigned char exdividendprice[15];	// 주식 및 주가지수 상품에만 적용됨 - 선물 : 배당락후미래가치 - 옵션 : 배당락후현재가치 예) 999999999.999999
	unsigned char lastpricesign[1];
	unsigned char lastprice[12];	// 예) 9999999999.99
	unsigned char lastpricetype[1];	// 1:실세; 3:거래무 2:기세 -2012.05.14 삭제
	unsigned char tradeprice[12];
	unsigned char highpricesign[1];	// 예) 9999999999.99
	unsigned char highprice[12];
	unsigned char lowpricesign[1];	// 예) 9999999999.99
	unsigned char lowprice[12];
	unsigned char firstcontractedtime[8];	// 예) 9999999999.99
	unsigned char lastcontractedtime[8];
	unsigned char lastsettlepricetype[2];
	unsigned char settletheordiffsign[1];	// *선물 10:정산가없음 11:당일종가(실세) 12:당일기세(거래성립후기세) - 2010.12.20 삭제 13:전일정산가(거래성립후 종가미형성) - 2010.12.20 삭제 14:당일이론가(거래성립후 종가미형성) 15:스프레드분 종가 - 2010.12.20 삭제 16:조정된 전일 정산가 - 2010.12.20 삭제 17:대상자산 종가(이론가없는 상품) 18:정산기준가격 - 2010.12.20 추가
	unsigned char settletheoreticdiff[12];
	unsigned char prevdayunsettledcontractnum[10];	// 에) 999999.999999
	unsigned char prevbestaskprice[12];
	unsigned char prevbestbidsign[1];
	unsigned char prevbestbidprice[12];	// 예) 9999999999.99
	unsigned char impliedvolatility[10];
	unsigned char underlyingmaxdelta[6];	// 예) 9999999999.99
	unsigned char alltimehighestsign[1];	// 예) 999999.9999
	unsigned char alltimehighestprice[12];	// 1:%; 2:절대값
	unsigned char alltimelowestsign[1];	// 예) 9999.99
	unsigned char alltimelowestprice[12];
	unsigned char annualhighestsign[1];	// 예) 9999999999.99
	unsigned char annualhighestprice[12];
	unsigned char annuallowestsign[1];	// 예) 9999999999.99
	unsigned char annuallowestprice[12];
	unsigned char alltimelowestdate[8];	// 예) 9999999999.99
	unsigned char annualhighestdate[8];
	unsigned char annuallowestdate[8];	// 예) 9999999999.99
	unsigned char annualbasedaysnum[8];
	unsigned char monthlytradedaysnum[8];
	unsigned char yearlytradedaysnum[8];
	unsigned char prevdaycontractsnum[16];
	unsigned char prevdayquantity[12];
	unsigned char prevdaytotalamount[22];
	unsigned char prevdayblocktradequantity[12];
	unsigned char prevdayblocktradeamount[22];
	unsigned char rate_cd[6];
	unsigned char maxunsettledcontractsnum[8];
	unsigned char affiliatedcommoditygroup[4];
	unsigned char limitorderconditioncode[1];
	unsigned char marketorderconditioncode[1];
	unsigned char conditionallimitorderconditioncode[1];
	unsigned char bestlimitorderconditioncode[1];
	unsigned char subjectedtoefpyn[1];
	unsigned char subjectedtoflexyn[1];
	unsigned char prevdayefpquantity[12];
	unsigned char prevdayefpamount[22];
	unsigned char closureyn[1];
	unsigned char filler[133];
	unsigned char eot[1];
};

struct KrxFuturesBestQuotation{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char marketstatecode[2];	// 최종 record '999999999999' set
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char bid1price[5];	// 에) KR4101C90009 → K101C9000
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];
	unsigned char bid2quantity[6];
	unsigned char bid3sign[1];
	unsigned char bid3price[5];
	unsigned char bid3quantity[6];	// F:원월종목; N:근월종목
	unsigned char bid4sign[1];	// C:현금결제; D:실물인수도결제
	unsigned char bid4price[5];
	unsigned char bid4quantity[6];	// 예) 9999999999.99
	unsigned char bid5sign[1];
	unsigned char bid5price[5];	// 예) 9999999999.99
	unsigned char bid5quantity[6];	// 예) 9999999999.99
	unsigned char asktotalquantity[6];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char ask1sign[1];	// A:미국형;E:유럽형;Z:기타
	unsigned char ask1price[5];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char ask1quantity[6];
	unsigned char ask2sign[1];
	unsigned char ask2price[5];
	unsigned char ask2quantity[6];
	unsigned char ask3sign[1];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char ask3price[5];	// 권리행사 가능한 최종일자
	unsigned char ask3quantity[6];	// 예) 999999999.99999999
	unsigned char ask4sign[1];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char ask4price[5];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char ask4quantity[6];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char ask5sign[1];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char ask5price[5];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char ask5quantity[6];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char bidtotalvalidnum[5];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	unsigned char bid1totalvalidnum[4];
	unsigned char bid2totalvalidnum[4];	// 예) 9999999999.99
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char asktotalvalidnum[5];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char ask1totalvalidnum[4];
	unsigned char ask2totalvalidnum[4];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char ask3totalvalidnum[4];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char ask4totalvalidnum[4];	// 예) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char timestamp[8];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char expectedpricesign[1];	// 예) 999999999.999999
	unsigned char expectedprice[5];	// 예) 999999999.99999999
	unsigned char eot[1];	//  Y; N
};

struct KrxFuturesTrade{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// 최종 record '999999999999' set
	unsigned char currentprice[5];
	unsigned char tradequantity[6];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char tradetype[2];	// 에) KR4101C90009 → K101C9000
	unsigned char timestamp[8];
	unsigned char closesttradeprice[5];
	unsigned char distanttradeprice[5];
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];	// F:원월종목; N:근월종목
	unsigned char lowpricesign[1];	// C:현금결제; D:실물인수도결제
	unsigned char lowprice[5];
	unsigned char lastpricesign[1];	// 예) 9999999999.99
	unsigned char lastprice[5];
	unsigned char accumtradequantity[7];	// 예) 9999999999.99
	unsigned char accumtradeamount[12];	// 예) 9999999999.99
	unsigned char eot[1];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
};

struct KrxFuturesTradeBestQuotation{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// 최종 record '999999999999' set
	unsigned char currentprice[5];
	unsigned char tradequantity[6];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char tradetype[2];	// 에) KR4101C90009 → K101C9000
	unsigned char timestamp[8];
	unsigned char closesttradeprice[5];
	unsigned char distanttradeprice[5];
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];	// F:원월종목; N:근월종목
	unsigned char lowpricesign[1];	// C:현금결제; D:실물인수도결제
	unsigned char lowprice[5];
	unsigned char lastpricesign[1];	// 예) 9999999999.99
	unsigned char lastprice[5];
	unsigned char accumtradequantity[7];	// 예) 9999999999.99
	unsigned char accumtradeamount[12];	// 예) 9999999999.99
	unsigned char marketstatecode[2];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char bidtotalquantity[6];	// A:미국형;E:유럽형;Z:기타
	unsigned char bid1sign[1];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];
	unsigned char bid2quantity[6];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char bid3sign[1];	// 권리행사 가능한 최종일자
	unsigned char bid3price[5];	// 예) 999999999.99999999
	unsigned char bid3quantity[6];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char bid4sign[1];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char bid4price[5];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char bid4quantity[6];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char bid5sign[1];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char bid5price[5];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char bid5quantity[6];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];	// 예) 9999999999.99
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char ask2sign[1];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char ask2price[5];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char ask2quantity[6];
	unsigned char ask3sign[1];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char ask3price[5];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char ask3quantity[6];	// 예) 999999999.99999999
	unsigned char ask4sign[1];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char ask4price[5];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char ask4quantity[6];	// 예) 999999999.999999
	unsigned char ask5sign[1];	// 예) 999999999.99999999
	unsigned char ask5price[5];	//  Y; N
	unsigned char ask5quantity[6];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 +5%를 계산한 가격) 예) 9999999999.99
	unsigned char bidtotalvalidnum[5];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 -5%를 계산한 가격) 예) 9999999999.99
	unsigned char bid1totalvalidnum[4];	// 예) 999999999.99999999
	unsigned char bid2totalvalidnum[4];	// 0:선물; 1:ATM; 2:ITM; 3:OTM
	unsigned char bid3totalvalidnum[4];	// Y; N
	unsigned char bid4totalvalidnum[4];	// 주식 및 주가지수 상품에만 적용됨 - 선물 : 배당락후미래가치 - 옵션 : 배당락후현재가치 예) 999999999.999999
	unsigned char bid5totalvalidnum[4];
	unsigned char asktotalvalidnum[5];	// 예) 9999999999.99
	unsigned char ask1totalvalidnum[4];	// 1:실세; 3:거래무 2:기세 -2012.05.14 삭제
	unsigned char ask2totalvalidnum[4];
	unsigned char ask3totalvalidnum[4];	// 예) 9999999999.99
	unsigned char ask4totalvalidnum[4];
	unsigned char ask5totalvalidnum[4];	// 예) 9999999999.99
	unsigned char eot[1];
};

struct KrxFuturesEnd{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char endpricesign[1];	// 최종 record '999999999999' set
	unsigned char endprice[5];
	unsigned char endpricecode[1];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char accumtradequantity[7];	// 에) KR4101C90009 → K101C9000
	unsigned char accumtradeamount[12];
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];	// F:원월종목; N:근월종목
	unsigned char bid2quantity[6];	// C:현금결제; D:실물인수도결제
	unsigned char bid3sign[1];
	unsigned char bid3price[5];	// 예) 9999999999.99
	unsigned char bid3quantity[6];
	unsigned char bid4sign[1];	// 예) 9999999999.99
	unsigned char bid4price[5];	// 예) 9999999999.99
	unsigned char bid4quantity[6];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char bid5sign[1];	// A:미국형;E:유럽형;Z:기타
	unsigned char bid5price[5];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char bid5quantity[6];
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char ask2sign[1];	// 권리행사 가능한 최종일자
	unsigned char ask2price[5];	// 예) 999999999.99999999
	unsigned char ask2quantity[6];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char ask3sign[1];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char ask3price[5];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char ask3quantity[6];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char ask4sign[1];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char ask4price[5];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char ask4quantity[6];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	unsigned char ask5sign[1];
	unsigned char ask5price[5];	// 예) 9999999999.99
	unsigned char ask5quantity[6];
	unsigned char bidtotalvalidnum[5];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char bid1totalvalidnum[4];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char bid2totalvalidnum[4];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char asktotalvalidnum[5];	// 예) 999999999.99999999
	unsigned char ask1totalvalidnum[4];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char ask2totalvalidnum[4];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char ask3totalvalidnum[4];	// 예) 999999999.999999
	unsigned char ask4totalvalidnum[4];	// 예) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	//  Y; N
	unsigned char eot[1];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 +5%를 계산한 가격) 예) 9999999999.99
};

struct KrxFuturesMarketManage{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char timestamp[8];
	unsigned char marketmanagecode[3];
	unsigned char stockid[10];	// 최종 record '999999999999' set
	char krcode[12];
	unsigned char seq[2];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char marketstatecode[2];	// 에) KR4101C90009 → K101C9000
	unsigned char bidaskexistyn[1];
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];	// F:원월종목; N:근월종목
	unsigned char bid2quantity[6];	// C:현금결제; D:실물인수도결제
	unsigned char bid3sign[1];
	unsigned char bid3price[5];	// 예) 9999999999.99
	unsigned char bid3quantity[6];
	unsigned char bid4sign[1];	// 예) 9999999999.99
	unsigned char bid4price[5];	// 예) 9999999999.99
	unsigned char bid4quantity[6];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char bid5sign[1];	// A:미국형;E:유럽형;Z:기타
	unsigned char bid5price[5];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char bid5quantity[6];
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char ask2sign[1];	// 권리행사 가능한 최종일자
	unsigned char ask2price[5];	// 예) 999999999.99999999
	unsigned char ask2quantity[6];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char ask3sign[1];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char ask3price[5];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char ask3quantity[6];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char ask4sign[1];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char ask4price[5];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char ask4quantity[6];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	unsigned char ask5sign[1];
	unsigned char ask5price[5];	// 예) 9999999999.99
	unsigned char ask5quantity[6];
	unsigned char bidtotalvalidnum[5];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char bid1totalvalidnum[4];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char bid2totalvalidnum[4];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char asktotalvalidnum[5];	// 예) 999999999.99999999
	unsigned char ask1totalvalidnum[4];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char ask2totalvalidnum[4];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char ask3totalvalidnum[4];	// 예) 999999999.999999
	unsigned char ask4totalvalidnum[4];	// 예) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	//  Y; N
	unsigned char eot[1];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 +5%를 계산한 가격) 예) 9999999999.99
};

struct KrxFuturesSpace{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char eot[1];
};

struct KrxFuturesDistribute{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char distribute_startorend[1];	// 최종 record '999999999999' set
	unsigned char distributetype[1];
	unsigned char distributeendtime[8];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char eot[1];	// 에) KR4101C90009 → K101C9000
};

struct KrxFuturesInvestorData{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char createdate[8];
	unsigned char createtime[6];
	unsigned char investordatatype[2];	// 최종 record '999999999999' set
	unsigned char stockid[10];
	unsigned char investortype[4];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char bidsettledquantity[9];	// 에) KR4101C90009 → K101C9000
	unsigned char asksettledquantity[9];
	unsigned char bidsettledamount[18];
	unsigned char asksettledamount[18];
	unsigned char bidspreadedquantity[9];
	unsigned char askspreadedquantity[9];
	unsigned char bidspreadedamount[18];
	unsigned char askspreadedamount[18];	// F:원월종목; N:근월종목
	unsigned char eot[1];	// C:현금결제; D:실물인수도결제
};

struct KrxFuturesUnsettled{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char unsettledtype[2];	// 최종 record '999999999999' set
	unsigned char tradedate[8];
	unsigned char unsettledamount[9];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char eot[1];	// 에) KR4101C90009 → K101C9000
};

struct KrxFuturesSettled{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char settledprice[16];	// 최종 record '999999999999' set
	unsigned char settledpricetype[1];
	unsigned char finalsettledprice[16];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char finalsettledpricetype[1];	// 에) KR4101C90009 → K101C9000
	unsigned char eot[1];
};

struct KrxFuturesMarketpriceRecovery{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// 최종 record '999999999999' set
	unsigned char currentprice[5];
	unsigned char closesttradeprice[5];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char distanttradeprice[5];	// 에) KR4101C90009 → K101C9000
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];
	unsigned char lowpricesign[1];
	unsigned char lowprice[5];
	unsigned char unsettledamount[9];	// F:원월종목; N:근월종목
	unsigned char accumtradequantity[7];	// C:현금결제; D:실물인수도결제
	unsigned char accumtradeamount[12];
	unsigned char ask1sign[1];	// 예) 9999999999.99
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 예) 9999999999.99
	unsigned char ask1num[4];	// 예) 9999999999.99
	unsigned char bid1sign[1];	// 예) K2I:KOSPI200지수; STI:스타지수; Sxx:개별주식; B03:3년국채; B05:5년국채; B10:10년국채; CDR:CD; MSB:통안채; USD:미국달러; JPY:일본엔; EUR:유럽유로; GLD:금; PMT:돈육
	unsigned char bid1price[5];	// A:미국형;E:유럽형;Z:기타
	unsigned char bid1quantity[6];	// T1:최근월+차근월 T2:최근월+2차근월 T3:최근월+3차근월 T4:최근월+4차근월 T5:최근월+5차근월 T6:최근월+6차근월
	unsigned char bid1num[4];
	unsigned char ask2sign[1];
	unsigned char ask2price[5];
	unsigned char ask2quantity[6];
	unsigned char ask2num[4];	// 1:최근월물;선물스프레드 2:2째월물 3:3째월물 4:4째월물 5:5째월물 6:6째월물 7:7째월물
	unsigned char bid2sign[1];	// 권리행사 가능한 최종일자
	unsigned char bid2price[5];	// 예) 999999999.99999999
	unsigned char bid2quantity[6];	// C:거래단위조정 N:조정없음 O:미결제조정
	unsigned char bid2num[4];	// 1계약에 해당하는 기초자산수량 예) 999999999.99999999
	unsigned char ask3sign[1];	// 약정대금 및 결제시 사용하는 계산승수 예) 9999999999999.99999999
	unsigned char ask3price[5];	// 0:미시장조성종목 1:당일시장조성종목 2:과거시장조성종목
	unsigned char ask3quantity[6];	// 1:신규상장 2:추가상장 3:기존종목 4:최초상장 5:종목조정 6:특별설정
	unsigned char ask3num[4];	// 기초자산기준가격에 가장 가까운 행사가격 예) 9999999999.99
	unsigned char bid3sign[1];	// 00:해당사항없음; 01:권리락; 02:배당락; 03:분배락; 04:권배락; 05:중간배당락; 06:권리중간배당락; 99:기타
	unsigned char bid3price[5];
	unsigned char bid3quantity[6];	// 예) 9999999999.99
	unsigned char bid3num[4];
	unsigned char ask4sign[1];	// 주식파생상품의 기초주권 조정기준가격 예) 999999999.99999999
	unsigned char ask4price[5];	// *선물 spcace:기준가 없음(선물 스프레드종목) 11:전일정산가 12:전일기준가(거래성립전 종가미형성) 13:당일이론가(거래성립전 종가미형성) 14:전일기세(거래성립전 기세형성) -2012.05.14 삭제 15:당일이론가(거래성립전 기세형성)  -2012.05.14 삭제 16:조정된 전일정산가 17:조정된 전일기준가 (거래성립전 종가미형성) 18:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제 19:전일 대상자산 종가(이론가없는 상품) *옵션 21:전일증거금기준가 22:전일기준가(거래성립전 종가미형성) 23:당일이론가(거래성립전 종가미형성) 24:전일기세(거래성립전 기세형성)   -2012.05.14 삭제 25:조정된 전일증거금기준가 26:조정된 전일기준가   (거래성립전 종가미형성) 27:조정된 전일기세(거래성립전 기세형성)   -2012.05.14 삭제
	unsigned char ask4quantity[6];	// 0:해당없음 1:실세 2:기세 3:이론가 4:대상자산종가
	unsigned char ask4num[4];
	unsigned char bid4sign[1];	// 주식파생상품의 조정종가 예) 999999999.99999999
	unsigned char bid4price[5];	// Y:협의매매상품; N:대상아님 가능품목:3년물국채선물; 미국달러선물; 유로선물; 엔선물
	unsigned char bid4quantity[6];	// 예) 999999999.99999999
	unsigned char bid4num[4];	// *옵션 20:증거금 기준가 없음 21:당일종가(실세) 22:당일기세(거래성립후기세) -2012.05.14 삭제 23:전일증거금기준가 (거래성립후 종가미형성)  -2012.05.14 삭제 24:당일이론가(거래성립후 종가미형성) 25:조정전일증거금기준가   (거래성립후 종가미형성)   -2012.05.14 삭제 25:조정전일증거금기준가   (거래성립후 종가미형성)
	unsigned char ask5sign[1];	// ※옵션의 경우 증거금기준가용 예) 999999999.999999
	unsigned char ask5price[5];	// 예) 999999999.999999
	unsigned char ask5quantity[6];	// 예) 999999999.99999999
	unsigned char ask5num[4];	//  Y; N
	unsigned char bid5sign[1];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 +5%를 계산한 가격) 예) 9999999999.99
	unsigned char bid5price[5];	// 선물 CB 발동조건을 판단하기 위한 상한 가격(기준가대비 +/-5%가 CB 발동 조건인 경우 -5%를 계산한 가격) 예) 9999999999.99
	unsigned char bid5quantity[6];	// 예) 999999999.99999999
	unsigned char bid5num[4];	// 0:선물; 1:ATM; 2:ITM; 3:OTM
	unsigned char asktotalquantity[6];	// Y; N
	unsigned char asktotalnum[5];	// 주식 및 주가지수 상품에만 적용됨 - 선물 : 배당락후미래가치 - 옵션 : 배당락후현재가치 예) 999999999.999999
	unsigned char bidtotalquantity[6];
	unsigned char bidtotalnum[5];	// 예) 9999999999.99
	unsigned char marketstatetype[2];	// 1:실세; 3:거래무 2:기세 -2012.05.14 삭제
	unsigned char marketmanagetype[3];
	unsigned char expectedpricesign[1];	// 예) 9999999999.99
	unsigned char expectedprice[5];
	unsigned char eot[1];	// 예) 9999999999.99
};

struct KrxFuturesPolling{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char timestampHHSS[4];
	unsigned char mainorbackup[2];
	unsigned char filler[8];	// 최종 record '999999999999' set
	unsigned char eot[1];
};

struct KrxFuturesOpenMarketManage{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200지수선물
	unsigned char markettype[1];	// 4
	unsigned char timestamp[8];
	unsigned char marketmanagecode[3];
	unsigned char stockid[10];	// 최종 record '999999999999' set
	char krcode[12];
	unsigned char seq[3];	// 예) KRSPIFUK2I:코스피200선물 KRSPIOPK2I:코스피200옵션 KREQUOPS46:KRX주식옵션 KB금융
	unsigned char marketstatecode[2];	// 에) KR4101C90009 → K101C9000
	unsigned char eot[1];
};
#endif
