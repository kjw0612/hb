#include <stdio.h>
#include "K200Code.h"

namespace K200CODE {

bool isLeapYear(int YYYY) {
	return YYYY%4 == 0 && (YYYY%100 != 0 || YYYY%400 == 0);
}

const static int mday[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
const static int acc_mday[13] = {0,
	31,
	31+28,
	31+28+31,
	31+28+31+30,
	31+28+31+30+31,
	31+28+31+30+31+30,
	31+28+31+30+31+30+31,
	31+28+31+30+31+30+31+31,
	31+28+31+30+31+30+31+31+30,
	31+28+31+30+31+30+31+31+30+31,
	31+28+31+30+31+30+31+31+30+31+30,
	31+28+31+30+31+30+31+31+30+31+30+31};

int getDayFromY1(int YYYY,int MM,int DD) {
	YYYY --;
	return 
		YYYY * 365 + 
		(YYYY / 4) - (YYYY / 100) + (YYYY / 400) + 
		acc_mday[MM-1] + DD + 
		((MM > 2 && isLeapYear(YYYY+1))?1:0);
}

int countForGetThursday(int yyyy,int mm,int dd) {
	 return (11-getDayFromY1(yyyy,mm,dd)%7)%7;
}

int getExpireThursday(int yyyy,int mm) {
	int firstThursday = countForGetThursday(yyyy, mm, 1) + 1;
	int expireThursday = firstThursday + 7;
	return expireThursday;
}

void getExpireDay(string day, Type type, int from, int&yyyy, int&mm, int&dd) {
	int iday = atoi(day.c_str());
	dd = iday % 100; iday /= 100;
	mm = iday % 100; iday /= 100;
	yyyy = iday;

	if (type == Future) {
		if (mm % 3 != 0) dd = 1;
		mm += (12-mm)%3;
	}

	int expDay = getExpireThursday(yyyy,mm);
	if (dd > expDay) {
		mm += ( (type == Future)?3:1);
		if (mm > 12) {
			mm -= 12; yyyy ++;
		}
	}

	mm += ( (type == Future)?3:1) * from;
	yyyy += (mm - 1) / 12;
	mm = (mm - 1) % 12 + 1;

	dd = getExpireThursday(yyyy, mm);
}

int getCheckNum(string &KRcode){
	int checkNum = 0;
	vector<int> xnum;
	for (int i=0;i<(int)KRcode.length();i++) {
		char c = KRcode.at(i);
		int value = 0;
		if ('0' <= c && c <= '9') value = c - '0';
		else if ('A' <= c && c <= 'Z') value = c - 'A' + 10;
		if (value >= 10) xnum.push_back(value / 10);
		xnum.push_back(value % 10);
	}
	int mul = 2;
	for (int i=((int)xnum.size()) - 1; i >= 0; i --) {
		int value = xnum[i] * mul;

		checkNum -= (value % 10 + value / 10);
		mul = 3-mul;
	}
	checkNum = (10 + checkNum % 10)%10;
	return checkNum;
}

const static char yearCode[] = "012345ABCDEFGHJKLMNPQRSTVW6789";
const static char monthCode[] = "?123456789ABC";
string getCode(string day, Type type, int from, int price) {
	int YYYY, MM, DD;

	getExpireDay(day, type, from, YYYY, MM, DD);
	string specCode = "";
	int x,y,z;
	x=y=z=0;
	if (type == Call || type == Put) {
		x = price / 100;
		y = (price / 10)%10;
		z = price%10;
	}
	specCode.push_back((char)x+'0');
	specCode.push_back((char)y+'0');
	specCode.push_back((char)z+'0');

	string KRcode = (string)"KR4"+(char)(type+'0')+"01"+yearCode[(YYYY+10)%30]+monthCode[MM]+specCode;
	int checkNum = getCheckNum(KRcode);

	return KRcode + (char)(checkNum + '0');
}

};