#ifndef brickbase_h__
#define brickbase_h__

#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "indexer.h"
#include "macros.h"
#include "singleton.h"
#include "basereaders.h"
#include <vector>

class Functional{
public:
	static void timestampfunctest(){
		for (int i=9000000;i<12000000;i+=1221212){
			int x = Functional::timestamp2seq(i);
			int y = Functional::seq2timestamp(x);
			printf("%d %d %d\n",i,x,y);
		}
		for (int x = Functional::timestamp2seq(9000000); x <= Functional::timestamp2seq(16000000); x += 123456){
			int y = Functional::seq2timestamp(x);
			printf("%d %d\n",x,y);
		}
	}

	inline static int timestamp2seq(int timestamp){
		// 9000000 // HHMMSSxx
		const int ss_xx = 100, MM_ss = 60, HH_MM = 60;
		int seq = 0;
		seq += timestamp % 100; // xx
		timestamp /= 100; // HHMMSS
		seq += timestamp % 100 * ss_xx; // ss * 100
		timestamp /= 100; // HHMM
		seq += (timestamp % 100) * MM_ss * ss_xx; // MM * 60 * 100
		timestamp /= 100; // HH
		seq += timestamp * HH_MM * MM_ss * ss_xx;
		return seq;
	}

	inline static int seq2timestamp(int seq){
		const int ss_xx = 100, MM_ss = 60, HH_MM = 60;
		int HH = ((int)seq / (ss_xx * MM_ss * HH_MM));
		int mm = ((int)seq / (ss_xx * MM_ss)) % HH_MM;
		int ss = ((int)seq / (ss_xx)) % MM_ss;
		int xx = seq % ss_xx;
		return ((HH * 100 + mm) * 100 + ss) * 100 + xx;
	}

	inline static int tsdiff(int timestampa, int timestampb){
		int sqa = timestamp2seq(timestampa), sqb = timestamp2seq(timestampb);
		return sqa - sqb;
	}

	inline static std::vector<int> makegrid(int timestamp_st, int timestamp_end, int nTimes){
		std::vector<int> grid(nTimes);
		int seq_st = Functional::timestamp2seq(timestamp_st);
		int seq_end = Functional::timestamp2seq(timestamp_end);
		int seq_i = 0, timestamp_i = 0;
		for (int i=0;i<nTimes;++i){
			seq_i = ((long long)seq_st * (nTimes-1-i) + (long long)seq_end * (i)) / (nTimes-1);
			timestamp_i = Functional::seq2timestamp(seq_i);
			grid[i] = timestamp_i;
		}
		return grid;
	}
	template<class T>
	inline static bool ptr_comp(const T * const & a, const T * const & b)
	{
		return *a < *b;
	}
};

class Interpolation{
public:
	template<class x_type>
	static int locate(const std::vector<x_type>& xs, x_type x)
	{
		if (x < xs.front())
			return 0;
		else if (x > xs.back())
			return xs.end()-xs.begin()-2;
		else
			return std::upper_bound(xs.begin(),xs.end()-1,x)-xs.begin()-1;
	}

	template<class x_type>
	static int locate_ptr(const std::vector<x_type *>& xs, x_type * x)
	{
		if (*(x) < *(xs.front()))
			return 0;
		else if (*(xs.back()) < *(x))
			return xs.end()-xs.begin()-2;
		else
			return std::upper_bound(xs.begin(),xs.end()-1,x,Functional::ptr_comp<x_type>)-xs.begin()-1;
	}

	// xs are sorted in ascending order
	template<class x_type, class y_type>
	static y_type linear(const std::vector<x_type>& xs, const std::vector<y_type>& ys, x_type x)
	{
		int i = Interpolation::locate(xs, x);
		if (i==0 || i+1==xs.size() || xs[i] == x){
			return ys[i];
		}
		else{
			return (ys[i] * (x-xs[i]) + ys[i+1] * (xs[i+1] - x)) / (xs[i+1] - xs[i]);
		}
	}

	static double linear_bricks(const std::vector<Brick *>& bricks, int x, bool prev, Brick::DataType dataType)
	{
		Brick brick; brick.pi.timestamp = x;
		int i = Interpolation::locate_ptr(bricks, &brick);
		if (i==0 || i+1==bricks.size() || bricks[i]->pi.timestamp == x){
			return bricks[i]->getValue(dataType);
		}
		else{
			if (prev){
				return bricks[i]->getValue(dataType);
			}
			else{
				return (bricks[i]->getValue(dataType) * Functional::tsdiff(x,bricks[i]->pi.timestamp) +
					bricks[i+1]->getValue(dataType) * Functional::tsdiff(bricks[i+1]->pi.timestamp,x))
					/ Functional::tsdiff(bricks[i+1]->pi.timestamp,bricks[i]->pi.timestamp);
			}
		}
	}
};

inline std::pair<std::vector<int>, std::vector<double> > bricks2MidPriceGrid
	(const std::vector<Brick *>& bricks, int mintime, int maxtime, int nTimes, Brick::DataType dataType = Brick::MidPrice)
{
	std::vector<int> grids = Functional::makegrid(mintime, maxtime, nTimes);
	std::vector<double> midprices(nTimes);
	for (int i=0;i<(int)grids.size();++i){
		midprices[i] = Interpolation::linear_bricks(bricks, grids[i], true, dataType);
	}
	return std::make_pair(grids, midprices);
}

inline std::pair<std::vector<int>, std::vector<double> > aXpbY
	(double a, const std::pair<std::vector<int>, std::vector<double> >& X,
	double b, const std::pair<std::vector<int>, std::vector<double> >& Y)
{
	std::pair<std::vector<int>, std::vector<double> > ret;
	if (X.first.size()==Y.first.size() && X.second.size()==Y.second.size()){
		ret.first = X.first;
		ret.second = std::vector<double>(X.second.size());
		for (int i=0;i<(int)ret.second.size();++i){
			ret.second[i] = a*X.second[i] + b*Y.second[i];
		}
	}
	return ret;
}

inline std::pair<std::vector<int>, std::vector<double> > aXpb
(double a, const std::pair<std::vector<int>, std::vector<double> >& X,
 double b)
{
	std::pair<std::vector<int>, std::vector<double> > ret;
	ret.first = X.first;
	ret.second = std::vector<double>(X.second.size());
	for (int i=0;i<(int)ret.second.size();++i){
		ret.second[i] = a*X.second[i] + b;
	}
	return ret;
}

class BrickBase{
public:
	BrickBase() {}
	BrickBase(const std::vector<Brick *>& brick)
	{
		for (int i=0;i<(int)brick.size();++i)
		{
			codeVectorMap[brick[i]->pi.krcodestr].push_back(brick[i]);
		}
		
		std::map<std::string, std::vector<Brick *> >::iterator it = codeVectorMap.begin();
		for (;it != codeVectorMap.end(); ++it){
			std::sort(it->second.begin(),it->second.end(),Functional::ptr_comp<Brick>);
			codes.push_back(it->first);
		}
	}

	void report_amounts(){
		std::map<std::string, std::vector<Brick *> >::iterator it = codeVectorMap.begin();
		for (;it != codeVectorMap.end(); ++it){
			std::cout << it->first << "\t" << it->second.size() << std::endl;
		}
	}
	const std::vector<Brick *>& get(const std::string& krcode){
		if (codeVectorMap.find(krcode) == codeVectorMap.end()){
			return empty_bricks;
		}
		return codeVectorMap.find(krcode)->second;
	}

	std::vector<Brick *> empty_bricks;
	std::map<std::string, std::vector<Brick *> > codeVectorMap;
	std::vector<std::string> codes;
};

#endif // brickbase_h__
