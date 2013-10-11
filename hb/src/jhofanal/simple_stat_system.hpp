#ifndef simple_stat_system_h__
#define simple_stat_system_h__

#include "teach_eval_scheme.hpp"
#include <map>
#include <algorithm>

class SimpleStatSystem : public Mto1System{
public:
	SimpleStatSystem(const vs& xnames_, int nn = 1) : Mto1System(xnames_, nn) {}
	static int hash(const vd& x){
		int n = (int)x.size();
		int ret = 0;
		for (int i=0;i<n;++i){
			ret *= 2; ret += (int)x[i]; }
		ret += 1<<(n+1);
		return ret;
	}
	void add(const vd& x, const vd& y){
		int hashx = hash(x);
		pi val = cases[hashx];
		if (hashy(y)) ++val.first;
		else ++val.second;
		cases[hashx] = val;
	}
	double error(const vd& x, const vd& y){
		int hashx = hash(x);
		map<int, pi>::iterator it = cases.lower_bound(hashx);
		pi val(1,0);
		if (it!=cases.end()){
			val = it->second;
		}
		double prob = val.first / (double)(val.first + val.second);
		double y_real = (double)hashy(y);
		return fabs(prob - y_real);
	}
	void optimize() {} // do nothing.
	map<int, pi> cases;
};


#endif // simple_stat_system_h__