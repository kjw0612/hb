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
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char stocksnum[5];
	unsigned char tradeday[8];
	char krcode[12];	// ���� record '999999999999' set
	unsigned char seq[6];
	char derivativeid[10];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	char krcodeshort[9];	// ��) KR4101C90009 �� K101C9000
	char koreanname[80];
	char koreannameshort[40];
	char englishname[80];
	char englishnameshort[40];
	unsigned char issuedate[8];
	unsigned char expirydate[8];
	unsigned char longorshortspread[1];	// F:��������; N:�ٿ�����
	unsigned char cashorasset[1];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char upperlimitsign[1];
	unsigned char upperlimitprice[12];	// ��) 9999999999.99
	unsigned char lowerlimitsign[1];
	unsigned char lowerlimitprice[12];	// ��) 9999999999.99
	unsigned char baseprice[12];	// ��) 9999999999.99
	unsigned char underlyingid[3];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char americanoreuropean[1];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char spreadtype[2];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char spreadnearbycode[12];
	unsigned char spreaddistantcode[12];
	unsigned char lasttradedate[8];
	unsigned char lastpaydate[8];
	unsigned char monthtype[1];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char lastexerdate[8];	// �Ǹ���� ������ ��������
	unsigned char strikeprice[17];	// ��) 999999999.99999999
	unsigned char adjusttype[1];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char tradingunit[17];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char tradingmultip[21];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char constructiontype[1];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char issuetype[1];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char atmprice[12];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char adjustreason[2];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	char underlyingkrcode[12];
	unsigned char underlyinglastprice[12];	// ��) 9999999999.99
	unsigned char timetomatdays[7];
	unsigned char adjustbaseprice[17];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char basepricetype[2];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char dealbasepricetype[1];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char lastadjpricesign[1];
	unsigned char lastadjprice[17];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char blocktradeyn[1];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char prevmarginbaseprice[17];	// ��) 999999999.99999999
	unsigned char prevmarginbasepricetype[2];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char settledtheoreticprice[15];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char basethereticprice[15];	// ��) 999999999.999999
	unsigned char prevsettledprice[17];	// ��) 999999999.99999999
	unsigned char prohibitedtradeyn[1];	//  Y; N
	unsigned char circuitbreakersupperlimit[12];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� +5%�� ����� ����) ��) 9999999999.99
	unsigned char circuitbreakerslowerlimit[12];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� -5%�� ����� ����) ��) 9999999999.99
	unsigned char inquirystrikeprice[17];	// ��) 999999999.99999999
	unsigned char moneynesstype[1];	// 0:����; 1:ATM; 2:ITM; 3:OTM
	unsigned char lastdayyn[1];	// Y; N
	unsigned char exdividendprice[15];	// �ֽ� �� �ְ����� ��ǰ���� ����� - ���� : �����Ĺ̷���ġ - �ɼ� : ���������簡ġ ��) 999999999.999999
	unsigned char lastpricesign[1];
	unsigned char lastprice[12];	// ��) 9999999999.99
	unsigned char lastpricetype[1];	// 1:�Ǽ�; 3:�ŷ��� 2:�⼼ -2012.05.14 ����
	unsigned char tradeprice[12];
	unsigned char highpricesign[1];	// ��) 9999999999.99
	unsigned char highprice[12];
	unsigned char lowpricesign[1];	// ��) 9999999999.99
	unsigned char lowprice[12];
	unsigned char firstcontractedtime[8];	// ��) 9999999999.99
	unsigned char lastcontractedtime[8];
	unsigned char lastsettlepricetype[2];
	unsigned char settletheordiffsign[1];	// *���� 10:���갡���� 11:��������(�Ǽ�) 12:���ϱ⼼(�ŷ������ı⼼) - 2010.12.20 ���� 13:�������갡(�ŷ������� ����������) - 2010.12.20 ���� 14:�����̷а�(�ŷ������� ����������) 15:��������� ���� - 2010.12.20 ���� 16:������ ���� ���갡 - 2010.12.20 ���� 17:����ڻ� ����(�̷а����� ��ǰ) 18:������ذ��� - 2010.12.20 �߰�
	unsigned char settletheoreticdiff[12];
	unsigned char prevdayunsettledcontractnum[10];	// ��) 999999.999999
	unsigned char prevbestaskprice[12];
	unsigned char prevbestbidsign[1];
	unsigned char prevbestbidprice[12];	// ��) 9999999999.99
	unsigned char impliedvolatility[10];
	unsigned char underlyingmaxdelta[6];	// ��) 9999999999.99
	unsigned char alltimehighestsign[1];	// ��) 999999.9999
	unsigned char alltimehighestprice[12];	// 1:%; 2:���밪
	unsigned char alltimelowestsign[1];	// ��) 9999.99
	unsigned char alltimelowestprice[12];
	unsigned char annualhighestsign[1];	// ��) 9999999999.99
	unsigned char annualhighestprice[12];
	unsigned char annuallowestsign[1];	// ��) 9999999999.99
	unsigned char annuallowestprice[12];
	unsigned char alltimelowestdate[8];	// ��) 9999999999.99
	unsigned char annualhighestdate[8];
	unsigned char annuallowestdate[8];	// ��) 9999999999.99
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
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char marketstatecode[2];	// ���� record '999999999999' set
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char bid1price[5];	// ��) KR4101C90009 �� K101C9000
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];
	unsigned char bid2quantity[6];
	unsigned char bid3sign[1];
	unsigned char bid3price[5];
	unsigned char bid3quantity[6];	// F:��������; N:�ٿ�����
	unsigned char bid4sign[1];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char bid4price[5];
	unsigned char bid4quantity[6];	// ��) 9999999999.99
	unsigned char bid5sign[1];
	unsigned char bid5price[5];	// ��) 9999999999.99
	unsigned char bid5quantity[6];	// ��) 9999999999.99
	unsigned char asktotalquantity[6];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char ask1sign[1];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char ask1price[5];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char ask1quantity[6];
	unsigned char ask2sign[1];
	unsigned char ask2price[5];
	unsigned char ask2quantity[6];
	unsigned char ask3sign[1];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char ask3price[5];	// �Ǹ���� ������ ��������
	unsigned char ask3quantity[6];	// ��) 999999999.99999999
	unsigned char ask4sign[1];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char ask4price[5];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char ask4quantity[6];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char ask5sign[1];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char ask5price[5];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char ask5quantity[6];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char bidtotalvalidnum[5];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	unsigned char bid1totalvalidnum[4];
	unsigned char bid2totalvalidnum[4];	// ��) 9999999999.99
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char asktotalvalidnum[5];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char ask1totalvalidnum[4];
	unsigned char ask2totalvalidnum[4];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char ask3totalvalidnum[4];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char ask4totalvalidnum[4];	// ��) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char timestamp[8];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char expectedpricesign[1];	// ��) 999999999.999999
	unsigned char expectedprice[5];	// ��) 999999999.99999999
	unsigned char eot[1];	//  Y; N
};

struct KrxFuturesTrade{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// ���� record '999999999999' set
	unsigned char currentprice[5];
	unsigned char tradequantity[6];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char tradetype[2];	// ��) KR4101C90009 �� K101C9000
	unsigned char timestamp[8];
	unsigned char closesttradeprice[5];
	unsigned char distanttradeprice[5];
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];	// F:��������; N:�ٿ�����
	unsigned char lowpricesign[1];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char lowprice[5];
	unsigned char lastpricesign[1];	// ��) 9999999999.99
	unsigned char lastprice[5];
	unsigned char accumtradequantity[7];	// ��) 9999999999.99
	unsigned char accumtradeamount[12];	// ��) 9999999999.99
	unsigned char eot[1];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
};

struct KrxFuturesTradeBestQuotation{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// ���� record '999999999999' set
	unsigned char currentprice[5];
	unsigned char tradequantity[6];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char tradetype[2];	// ��) KR4101C90009 �� K101C9000
	unsigned char timestamp[8];
	unsigned char closesttradeprice[5];
	unsigned char distanttradeprice[5];
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];	// F:��������; N:�ٿ�����
	unsigned char lowpricesign[1];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char lowprice[5];
	unsigned char lastpricesign[1];	// ��) 9999999999.99
	unsigned char lastprice[5];
	unsigned char accumtradequantity[7];	// ��) 9999999999.99
	unsigned char accumtradeamount[12];	// ��) 9999999999.99
	unsigned char marketstatecode[2];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char bidtotalquantity[6];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char bid1sign[1];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];
	unsigned char bid2quantity[6];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char bid3sign[1];	// �Ǹ���� ������ ��������
	unsigned char bid3price[5];	// ��) 999999999.99999999
	unsigned char bid3quantity[6];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char bid4sign[1];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char bid4price[5];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char bid4quantity[6];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char bid5sign[1];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char bid5price[5];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char bid5quantity[6];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];	// ��) 9999999999.99
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char ask2sign[1];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char ask2price[5];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char ask2quantity[6];
	unsigned char ask3sign[1];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char ask3price[5];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char ask3quantity[6];	// ��) 999999999.99999999
	unsigned char ask4sign[1];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char ask4price[5];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char ask4quantity[6];	// ��) 999999999.999999
	unsigned char ask5sign[1];	// ��) 999999999.99999999
	unsigned char ask5price[5];	//  Y; N
	unsigned char ask5quantity[6];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� +5%�� ����� ����) ��) 9999999999.99
	unsigned char bidtotalvalidnum[5];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� -5%�� ����� ����) ��) 9999999999.99
	unsigned char bid1totalvalidnum[4];	// ��) 999999999.99999999
	unsigned char bid2totalvalidnum[4];	// 0:����; 1:ATM; 2:ITM; 3:OTM
	unsigned char bid3totalvalidnum[4];	// Y; N
	unsigned char bid4totalvalidnum[4];	// �ֽ� �� �ְ����� ��ǰ���� ����� - ���� : �����Ĺ̷���ġ - �ɼ� : ���������簡ġ ��) 999999999.999999
	unsigned char bid5totalvalidnum[4];
	unsigned char asktotalvalidnum[5];	// ��) 9999999999.99
	unsigned char ask1totalvalidnum[4];	// 1:�Ǽ�; 3:�ŷ��� 2:�⼼ -2012.05.14 ����
	unsigned char ask2totalvalidnum[4];
	unsigned char ask3totalvalidnum[4];	// ��) 9999999999.99
	unsigned char ask4totalvalidnum[4];
	unsigned char ask5totalvalidnum[4];	// ��) 9999999999.99
	unsigned char eot[1];
};

struct KrxFuturesEnd{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char endpricesign[1];	// ���� record '999999999999' set
	unsigned char endprice[5];
	unsigned char endpricecode[1];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char accumtradequantity[7];	// ��) KR4101C90009 �� K101C9000
	unsigned char accumtradeamount[12];
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];	// F:��������; N:�ٿ�����
	unsigned char bid2quantity[6];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char bid3sign[1];
	unsigned char bid3price[5];	// ��) 9999999999.99
	unsigned char bid3quantity[6];
	unsigned char bid4sign[1];	// ��) 9999999999.99
	unsigned char bid4price[5];	// ��) 9999999999.99
	unsigned char bid4quantity[6];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char bid5sign[1];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char bid5price[5];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char bid5quantity[6];
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char ask2sign[1];	// �Ǹ���� ������ ��������
	unsigned char ask2price[5];	// ��) 999999999.99999999
	unsigned char ask2quantity[6];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char ask3sign[1];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char ask3price[5];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char ask3quantity[6];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char ask4sign[1];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char ask4price[5];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char ask4quantity[6];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	unsigned char ask5sign[1];
	unsigned char ask5price[5];	// ��) 9999999999.99
	unsigned char ask5quantity[6];
	unsigned char bidtotalvalidnum[5];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char bid1totalvalidnum[4];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char bid2totalvalidnum[4];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char asktotalvalidnum[5];	// ��) 999999999.99999999
	unsigned char ask1totalvalidnum[4];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char ask2totalvalidnum[4];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char ask3totalvalidnum[4];	// ��) 999999999.999999
	unsigned char ask4totalvalidnum[4];	// ��) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	//  Y; N
	unsigned char eot[1];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� +5%�� ����� ����) ��) 9999999999.99
};

struct KrxFuturesMarketManage{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char timestamp[8];
	unsigned char marketmanagecode[3];
	unsigned char stockid[10];	// ���� record '999999999999' set
	char krcode[12];
	unsigned char seq[2];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char marketstatecode[2];	// ��) KR4101C90009 �� K101C9000
	unsigned char bidaskexistyn[1];
	unsigned char bidtotalquantity[6];
	unsigned char bid1sign[1];
	unsigned char bid1price[5];
	unsigned char bid1quantity[6];
	unsigned char bid2sign[1];
	unsigned char bid2price[5];	// F:��������; N:�ٿ�����
	unsigned char bid2quantity[6];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char bid3sign[1];
	unsigned char bid3price[5];	// ��) 9999999999.99
	unsigned char bid3quantity[6];
	unsigned char bid4sign[1];	// ��) 9999999999.99
	unsigned char bid4price[5];	// ��) 9999999999.99
	unsigned char bid4quantity[6];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char bid5sign[1];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char bid5price[5];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char bid5quantity[6];
	unsigned char asktotalquantity[6];
	unsigned char ask1sign[1];
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char ask2sign[1];	// �Ǹ���� ������ ��������
	unsigned char ask2price[5];	// ��) 999999999.99999999
	unsigned char ask2quantity[6];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char ask3sign[1];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char ask3price[5];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char ask3quantity[6];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char ask4sign[1];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char ask4price[5];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char ask4quantity[6];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	unsigned char ask5sign[1];
	unsigned char ask5price[5];	// ��) 9999999999.99
	unsigned char ask5quantity[6];
	unsigned char bidtotalvalidnum[5];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char bid1totalvalidnum[4];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char bid2totalvalidnum[4];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char bid3totalvalidnum[4];
	unsigned char bid4totalvalidnum[4];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char bid5totalvalidnum[4];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char asktotalvalidnum[5];	// ��) 999999999.99999999
	unsigned char ask1totalvalidnum[4];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char ask2totalvalidnum[4];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char ask3totalvalidnum[4];	// ��) 999999999.999999
	unsigned char ask4totalvalidnum[4];	// ��) 999999999.99999999
	unsigned char ask5totalvalidnum[4];	//  Y; N
	unsigned char eot[1];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� +5%�� ����� ����) ��) 9999999999.99
};

struct KrxFuturesSpace{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char eot[1];
};

struct KrxFuturesDistribute{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char distribute_startorend[1];	// ���� record '999999999999' set
	unsigned char distributetype[1];
	unsigned char distributeendtime[8];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char eot[1];	// ��) KR4101C90009 �� K101C9000
};

struct KrxFuturesInvestorData{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char createdate[8];
	unsigned char createtime[6];
	unsigned char investordatatype[2];	// ���� record '999999999999' set
	unsigned char stockid[10];
	unsigned char investortype[4];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char bidsettledquantity[9];	// ��) KR4101C90009 �� K101C9000
	unsigned char asksettledquantity[9];
	unsigned char bidsettledamount[18];
	unsigned char asksettledamount[18];
	unsigned char bidspreadedquantity[9];
	unsigned char askspreadedquantity[9];
	unsigned char bidspreadedamount[18];
	unsigned char askspreadedamount[18];	// F:��������; N:�ٿ�����
	unsigned char eot[1];	// C:���ݰ���; D:�ǹ��μ�������
};

struct KrxFuturesUnsettled{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char unsettledtype[2];	// ���� record '999999999999' set
	unsigned char tradedate[8];
	unsigned char unsettledamount[9];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char eot[1];	// ��) KR4101C90009 �� K101C9000
};

struct KrxFuturesSettled{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char settledprice[16];	// ���� record '999999999999' set
	unsigned char settledpricetype[1];
	unsigned char finalsettledprice[16];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char finalsettledpricetype[1];	// ��) KR4101C90009 �� K101C9000
	unsigned char eot[1];
};

struct KrxFuturesMarketpriceRecovery{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	char krcode[12];
	unsigned char seq[2];
	unsigned char currentpricesign[1];	// ���� record '999999999999' set
	unsigned char currentprice[5];
	unsigned char closesttradeprice[5];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char distanttradeprice[5];	// ��) KR4101C90009 �� K101C9000
	unsigned char marketpricesign[1];
	unsigned char marketprice[5];
	unsigned char highpricesign[1];
	unsigned char highprice[5];
	unsigned char lowpricesign[1];
	unsigned char lowprice[5];
	unsigned char unsettledamount[9];	// F:��������; N:�ٿ�����
	unsigned char accumtradequantity[7];	// C:���ݰ���; D:�ǹ��μ�������
	unsigned char accumtradeamount[12];
	unsigned char ask1sign[1];	// ��) 9999999999.99
	unsigned char ask1price[5];
	unsigned char ask1quantity[6];	// ��) 9999999999.99
	unsigned char ask1num[4];	// ��) 9999999999.99
	unsigned char bid1sign[1];	// ��) K2I:KOSPI200����; STI:��Ÿ����; Sxx:�����ֽ�; B03:3�ⱹä; B05:5�ⱹä; B10:10�ⱹä; CDR:CD; MSB:���ä; USD:�̱��޷�; JPY:�Ϻ���; EUR:��������; GLD:��; PMT:����
	unsigned char bid1price[5];	// A:�̱���;E:������;Z:��Ÿ
	unsigned char bid1quantity[6];	// T1:�ֱٿ�+���ٿ� T2:�ֱٿ�+2���ٿ� T3:�ֱٿ�+3���ٿ� T4:�ֱٿ�+4���ٿ� T5:�ֱٿ�+5���ٿ� T6:�ֱٿ�+6���ٿ�
	unsigned char bid1num[4];
	unsigned char ask2sign[1];
	unsigned char ask2price[5];
	unsigned char ask2quantity[6];
	unsigned char ask2num[4];	// 1:�ֱٿ���;������������ 2:2°���� 3:3°���� 4:4°���� 5:5°���� 6:6°���� 7:7°����
	unsigned char bid2sign[1];	// �Ǹ���� ������ ��������
	unsigned char bid2price[5];	// ��) 999999999.99999999
	unsigned char bid2quantity[6];	// C:�ŷ��������� N:�������� O:�̰�������
	unsigned char bid2num[4];	// 1��࿡ �ش��ϴ� �����ڻ���� ��) 999999999.99999999
	unsigned char ask3sign[1];	// ������� �� ������ ����ϴ� ���¼� ��) 9999999999999.99999999
	unsigned char ask3price[5];	// 0:�̽����������� 1:���Ͻ����������� 2:���Ž�����������
	unsigned char ask3quantity[6];	// 1:�űԻ��� 2:�߰����� 3:�������� 4:���ʻ��� 5:�������� 6:Ư������
	unsigned char ask3num[4];	// �����ڻ���ذ��ݿ� ���� ����� ��簡�� ��) 9999999999.99
	unsigned char bid3sign[1];	// 00:�ش���׾���; 01:�Ǹ���; 02:����; 03:�й��; 04:�ǹ��; 05:�߰�����; 06:�Ǹ��߰�����; 99:��Ÿ
	unsigned char bid3price[5];
	unsigned char bid3quantity[6];	// ��) 9999999999.99
	unsigned char bid3num[4];
	unsigned char ask4sign[1];	// �ֽ��Ļ���ǰ�� �����ֱ� �������ذ��� ��) 999999999.99999999
	unsigned char ask4price[5];	// *���� spcace:���ذ� ����(���� ������������) 11:�������갡 12:���ϱ��ذ�(�ŷ������� ����������) 13:�����̷а�(�ŷ������� ����������) 14:���ϱ⼼(�ŷ������� �⼼����) -2012.05.14 ���� 15:�����̷а�(�ŷ������� �⼼����)  -2012.05.14 ���� 16:������ �������갡 17:������ ���ϱ��ذ� (�ŷ������� ����������) 18:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 19:���� ����ڻ� ����(�̷а����� ��ǰ) *�ɼ� 21:�������űݱ��ذ� 22:���ϱ��ذ�(�ŷ������� ����������) 23:�����̷а�(�ŷ������� ����������) 24:���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ���� 25:������ �������űݱ��ذ� 26:������ ���ϱ��ذ�   (�ŷ������� ����������) 27:������ ���ϱ⼼(�ŷ������� �⼼����)   -2012.05.14 ����
	unsigned char ask4quantity[6];	// 0:�ش���� 1:�Ǽ� 2:�⼼ 3:�̷а� 4:����ڻ�����
	unsigned char ask4num[4];
	unsigned char bid4sign[1];	// �ֽ��Ļ���ǰ�� �������� ��) 999999999.99999999
	unsigned char bid4price[5];	// Y:���ǸŸŻ�ǰ; N:���ƴ� ����ǰ��:3�⹰��ä����; �̱��޷�����; ���μ���; ������
	unsigned char bid4quantity[6];	// ��) 999999999.99999999
	unsigned char bid4num[4];	// *�ɼ� 20:���ű� ���ذ� ���� 21:��������(�Ǽ�) 22:���ϱ⼼(�ŷ������ı⼼) -2012.05.14 ���� 23:�������űݱ��ذ� (�ŷ������� ����������)  -2012.05.14 ���� 24:�����̷а�(�ŷ������� ����������) 25:�����������űݱ��ذ�   (�ŷ������� ����������)   -2012.05.14 ���� 25:�����������űݱ��ذ�   (�ŷ������� ����������)
	unsigned char ask5sign[1];	// �ؿɼ��� ��� ���űݱ��ذ��� ��) 999999999.999999
	unsigned char ask5price[5];	// ��) 999999999.999999
	unsigned char ask5quantity[6];	// ��) 999999999.99999999
	unsigned char ask5num[4];	//  Y; N
	unsigned char bid5sign[1];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� +5%�� ����� ����) ��) 9999999999.99
	unsigned char bid5price[5];	// ���� CB �ߵ������� �Ǵ��ϱ� ���� ���� ����(���ذ���� +/-5%�� CB �ߵ� ������ ��� -5%�� ����� ����) ��) 9999999999.99
	unsigned char bid5quantity[6];	// ��) 999999999.99999999
	unsigned char bid5num[4];	// 0:����; 1:ATM; 2:ITM; 3:OTM
	unsigned char asktotalquantity[6];	// Y; N
	unsigned char asktotalnum[5];	// �ֽ� �� �ְ����� ��ǰ���� ����� - ���� : �����Ĺ̷���ġ - �ɼ� : ���������簡ġ ��) 999999999.999999
	unsigned char bidtotalquantity[6];
	unsigned char bidtotalnum[5];	// ��) 9999999999.99
	unsigned char marketstatetype[2];	// 1:�Ǽ�; 3:�ŷ��� 2:�⼼ -2012.05.14 ����
	unsigned char marketmanagetype[3];
	unsigned char expectedpricesign[1];	// ��) 9999999999.99
	unsigned char expectedprice[5];
	unsigned char eot[1];	// ��) 9999999999.99
};

struct KrxFuturesPolling{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char timestampHHSS[4];
	unsigned char mainorbackup[2];
	unsigned char filler[8];	// ���� record '999999999999' set
	unsigned char eot[1];
};

struct KrxFuturesOpenMarketManage{
	unsigned char datatype[2];	// A0
	unsigned char infotype[2];	// 01:KOSPI200��������
	unsigned char markettype[1];	// 4
	unsigned char timestamp[8];
	unsigned char marketmanagecode[3];
	unsigned char stockid[10];	// ���� record '999999999999' set
	char krcode[12];
	unsigned char seq[3];	// ��) KRSPIFUK2I:�ڽ���200���� KRSPIOPK2I:�ڽ���200�ɼ� KREQUOPS46:KRX�ֽĿɼ� KB����
	unsigned char marketstatecode[2];	// ��) KR4101C90009 �� K101C9000
	unsigned char eot[1];
};
#endif
