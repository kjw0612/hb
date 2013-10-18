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
			ret *= 3;
			ret += ((int)x[i]+1);
		}
		//ret += (int)pow(3,n);
		return ret;
	}
	void add(const vd& x, const vd& y){
		int hashx = hash(x);
		pi val = cases[hashx];
		if (hashy(y)) ++val.first;
		else ++val.second;
		cases[hashx] = val;
	}
	double expect(const vd& x) const{
		int hashx = hash(x);
		map<int, pi>::const_iterator it = cases.lower_bound(hashx);
		pi val(1,0);
		if (it!=cases.end()){
			val = it->second;
		}
		double prob = val.first / (double)(val.first + val.second);
		return prob;
	}
	void print(){
		map<int, pi>::const_iterator it;
		for (it = cases.begin();it != cases.end(); ++it){
			int case_i = it->first;
			pi val_i = it->second;
			char buf[20]="";
			_itoa(case_i,buf,3);
			printf("(%s,%d,%d) ", buf, val_i.first, val_i.second);
		}
		printf("\n");
	}
	void lazyOptimize() {
		//print();
	} // do nothing.
	map<int, pi> cases;
};


#endif // simple_stat_system_h__