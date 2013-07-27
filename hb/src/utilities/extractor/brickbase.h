#ifndef brickbase_h__
#define brickbase_h__

#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "indexer.h"
#include "macros.h"
#include "singleton.h"
#include "basereaders.h"
#include <vector>
#include <iostream>
#include "functional.h"

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

	static double linear_bricks(const std::vector<Brick *>& bricks, int x, bool prev, Brick::GetDataType dataType)
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

inline std::vector<Brick *> bricks2Grid
	(const std::vector<Brick *>& bricks, int mintime, int maxtime, int nTimes)
{
	std::vector<Brick *> ret;
	std::vector<int> grids = Functional::makegrid(mintime, maxtime, nTimes);
	for (int i=0;i<(int)grids.size();++i){
		Brick key;
		key.pi.timestamp = grids[i];
		ret.push_back(*std::lower_bound(bricks.begin(),bricks.end(),&key,Functional::ptr_comp<Brick>));
	}
	return ret;
}

inline std::vector<Brick *> bricks2GridRdtsc
	(const std::vector<Brick *>& bricks, long long mintime_rdtsc, long long maxtime_rdtsc, int nTimes)
{
	std::vector<Brick *> ret;
	std::vector<long long> grids = Functional::makegrid_ll(mintime_rdtsc, maxtime_rdtsc, nTimes);
	for (int i=0;i<(int)grids.size();++i){
		Brick key;
		key.pi.rdtscStamp = grids[i];
		ret.push_back(*std::lower_bound(bricks.begin(),bricks.end(),&key,&Brick::rdtscCompPtr));
	}
	return ret;
}



inline std::pair<std::vector<int>, std::vector<double> > bricks2MidPriceGrid
	(const std::vector<Brick *>& bricks, int mintime, int maxtime, int nTimes, Brick::GetDataType dataType = Brick::MidPrice)
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

	std::vector<Brick *> gets(const std::vector<std::string>& krcodes){
		std::vector<Brick *> ret;
		for (int i=0;i<(int)krcodes.size();++i){
			const std::vector<Brick *>& tmp = this->get(krcodes[i]);
			ret.insert(ret.end(),tmp.begin(),tmp.end());
		}
		return ret;
	}

	std::vector<Brick *> getAll()
	{
		std::vector<Brick *> ret;
		std::map<std::string, std::vector<Brick *> >::const_iterator it = codeVectorMap.begin();
		for (;it!=codeVectorMap.end();++it){
			for (int i=0;i<(int)it->second.size();++i){
				it->second[i]->setKrCode(it->first);
			}
			std::copy(it->second.begin(),it->second.end(), std::back_inserter(ret));
		}
		return ret;
	}

	std::vector<Brick *> empty_bricks;
	std::map<std::string, std::vector<Brick *> > codeVectorMap;
	std::vector<std::string> codes;
};

#endif // brickbase_h__
