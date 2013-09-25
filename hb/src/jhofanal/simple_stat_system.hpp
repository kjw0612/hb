#ifndef simple_stat_system_h__
#define simple_stat_system_h__

#include "teach_eval_scheme.hpp"
#include <map>
#include <algorithm>

class SimpleStatSystem : public LearningSystem{
public:
	SimpleStatSystem(const vs& xnames_) : xnames_(xnames_) {}
	vs xnames(){ return xnames_; }

	static int hash(const vi& x){
		int n = (int)x.size();
		int ret = 0;
		for (int i=0;i<n;++i){
			ret *= 2; ret += x[i]; }
		ret += 1<<(n+1);
		return ret;
	}
	static int hashy(const vi& y){
		assert(y.size()==1); // only damn single value suitable as input.
		return (y[0] > 0);
	}
	void add(const vi& x, const vi& y){
		int hashx = hash(x);
		pi val = cases[hashx];
		if (hashy(y)) ++val.first;
		else ++val.second;
		cases[hashx] = val;
	}
	double error(const vi& x, const vi& y){
		int hashx = hash(x);
		map<int, pi>::iterator it = cases.lower_bound(hashx);
		pi val = it->second;
		double prob = val.first / (double)(val.first + val.second);
		double y_real = (double)hashy(y);
		return (prob - y_real) > 0 ? (prob - y_real) : -(prob - y_real);
	}
	void optimize() {} // do nothing.

	vs xnames_;
	map<int, pi> cases;
};


#endif // simple_stat_system_h__