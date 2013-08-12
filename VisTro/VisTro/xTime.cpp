#include "stdafx.h"
#include "xTime.h"

#include <algorithm>
#include <string>
using namespace std;

xTime::xTime() {
}
xTime::xTime(LL tvalue) {
	nsec = tvalue % 1000 * 1000; tvalue /= 1000;
	msec = tvalue % 1000; tvalue /= 1000;
	sec = tvalue % 60; tvalue /= 60;
	min = tvalue % 60; tvalue /= 60;
	hour = tvalue % 24; tvalue /= 24;

	hour = hour + 9;
	dayfrom1970 = tvalue;

	if (hour >= 24) {
		hour -= 24;
		dayfrom1970 ++;
	}
}


inline bool isleapyear(int yy) {
	return yy%4==0 && (yy%100 != 0 || yy%400 == 0);
}

inline int dayforyear(int year) {
	return (year-1)*365 + (year-1)/4 - (year-1)/100 + (year-1)/400;
}

const int mday[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
void xTime::setDay(string D) {
	int year = atoi(D.substr(0, 4).c_str());
	int month = atoi(D.substr(4, 2).c_str());
	int day = atoi(D.substr(6, 2).c_str());
	dayfrom1970 = dayforyear(year) - dayforyear(1970);
	for (int i=0;i<month-1;i++){
		dayfrom1970 += mday[i];
		if (i == 1) dayfrom1970 += (int)isleapyear(year);
	}
	dayfrom1970 += day-1;
}
void xTime::setTime(string T) {
	while (T.size() < 15) T += "0";
	hour = atoi(T.substr(0, 2).c_str());
	min = atoi(T.substr(2, 2).c_str());
	sec = atoi(T.substr(4, 2).c_str());
	msec = atoi(T.substr(6, 3).c_str());
	nsec = atoi(T.substr(9, 6).c_str());
}
xTime::xTime(string S) { // yyyyMMddhhmmssSSSSSSSSS
	assert(S.length() >= 14);
	if (S.length() >= 25 && S[16] == '.') {
		S = S.substr(0, 16) + S.substr(17, 8);
	}
	while(S.length() < 23) S += "0";
	setDay(S.substr(0, 8));
	if (S[8] >= '3') S = S.substr(0, 8) + "0" + S.substr(8);
	setTime(S.substr(8, 15));
}

string xTime::toString() {
	string res = "";
	char buf[64];
	int year, month, day;

	LL rday = dayfrom1970;
	for (year=1970; ;year++) {
		int uday = 365 + (int)isleapyear(year);
		if (rday >= uday) rday -= uday;
		else break;
	}
	for (month=0;month<12;month++) {
		int uday = mday[month];
		if (month == 1) uday += (int)isleapyear(year);
		if (rday >= uday) rday -= uday;
		else break;
	}
	month ++;
	day = (int)(rday + 1);

	sprintf_s(buf, 64, "%04d%02d%02d%02d%02d%02d%03d%06d", year, month, day, hour, min, sec, msec, nsec);
	res = buf;
	return res;
}

string xTime::toView() {
	string S = toString();
	string res = S.substr(0, 4)+"."+
		S.substr(4, 2)+"."+
		S.substr(6, 2)+" "+

		S.substr(8, 2)+":"+
		S.substr(10, 2)+":"+
		S.substr(12, 2)+" "+

		S.substr(14, 3)+" "+
		S.substr(17);

	return res;
}

LL xTime::getTime() {
	LL res = dayfrom1970;

	res = 24*res + (hour-9);
	res = 60*res + min;
	res = 60*res + sec;
	res = 1000*res + msec;
	res = 1000000*res + nsec;
	return res;
}