#ifndef __ANALYSE__ITOOLS__H__
#define __ANALYSE__ITOOLS__H__
//#define stoi(a) atoi((a).c_str())
#include "STL.h"
int stoi(string &S);
long long stoi64(string &S);

#define DIR_BID -1
#define DIR_ASK +1

#define SZ(a) ((int)a.size())
#define LL long long

const int BUFSIZE = 4096;
#endif//__ANALYSE__ITOOLS__H__