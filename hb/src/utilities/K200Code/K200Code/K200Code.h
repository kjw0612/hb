#pragma once

#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#ifdef K200CODE_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport) 
#else
#define MATHFUNCSDLL_API __declspec(dllimport) 
#endif

namespace K200CODE {
	enum Type {
		Future=1,
		Call=2,
		Put=3,
	};

	MATHFUNCSDLL_API string getCode(string day, Type type, int from, int price=0);
	MATHFUNCSDLL_API void getExpireDay(string day, Type type, int from, int&yyyy, int&mm, int&dd);
};