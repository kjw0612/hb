#ifndef simple_stat_system_h__
#define simple_stat_system_h__

#include "teach_eval_scheme.hpp"
#include <map>
#include <algorithm>

class SimpleStatSystem : public LearningSystem{
public:
	SimpleStatSystem(const vs& xnames_, int nn = 1) : xnames_(xnames_), nn(nn) {}
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
		//assert(y.size()==1); // only damn single value suitable as input.
		int ret = 0;
		for (int i=0;i<(int)y.size();++i) ret += y[i];
		return (ret > 0);
	}
	void add(const vi& x, const vi& y){
		int hashx = hash(x);
		pi val = cases[hashx];
		if (hashy(y)) ++val.first;
		else ++val.second;
		cases[hashx] = val;
	}

	vvi remakeX(const vvi& xs){
		vvi ret;
		for (int i=0;i+nn<(int)xs.size();i+=nn){
			vi vr;
			for (int j=i;j<i+nn;++j){
				vr.insert(vr.end(),xs[j].begin(),xs[j].end());
			}
			ret.push_back(vr);
		}
		return ret;
	}
	vvi remakeY(const vvi& ys){
		vvi ret;
		for (int i=0;i+nn<(int)ys.size();i+=nn){
			ret.push_back(ys[i+nn-1]);
		}
		return ret;
	}

	void adds(const vvi& xs, const vvi& ys){
		if (nn==1) LearningSystem::adds(xs,ys);
		else LearningSystem::adds(remakeX(xs),remakeY(ys));
	}
	double error(const vi& x, const vi& y){
		int hashx = hash(x);
		map<int, pi>::iterator it = cases.lower_bound(hashx);
		pi val = it->second;
		double prob = val.first / (double)(val.first + val.second);
		double y_real = (double)hashy(y);
		return (prob - y_real) > 0 ? (prob - y_real) : -(prob - y_real);
	}
	pair<double, int> errors(const vvi& xs, const vvi& ys){
		if (nn==1) return LearningSystem::errors(xs,ys);
		else return LearningSystem::errors(remakeX(xs),remakeY(ys));
	}
	void optimize() {} // do nothing.

	vs xnames_;
	int nn;
	map<int, pi> cases;
};


#endif // simple_stat_system_h__