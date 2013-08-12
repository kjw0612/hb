#ifndef __ANALYSE__XTIME__H__
#define __ANALYSE__XTIME__H__

#include <string>

typedef long long LL;

class xTime {
public:
	xTime();
	xTime(LL tvalue);
	xTime(std::string S); // yyyyMMddhhmmss etc

	std::string toString();
	std::string toView();
	void setDay(std::string D);
	void setTime(std::string T);
	LL getTime();
//private:
	LL dayfrom1970; // from 1970/1/1
	int hour, min, sec, msec, nsec;
	
};

#include <stdlib.h>
#include <assert.h>

#endif//__ANALYSE__XTIME__H__