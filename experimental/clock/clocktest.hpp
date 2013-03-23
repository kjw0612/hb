/*
 * clocktest.hpp
 *
 *  Created on: 2013. 3. 18.
 *      Author: koval
 */

#ifndef CLOCKTEST_HPP_
#define CLOCKTEST_HPP_

#include <time.h>
#include <unistd.h>
#include <chrono>
#include "cpufreq.hpp"
#include <functional>
#include <vector>
#include "plplot/plplot.h"
#include "plplot/plstream.h"
#include <cmath>

struct statistics{
	statistics(std::vector<uint64_t>& samples)
	: nDivision(50)
	{
		xmin = samples[0];
		xmax = samples[0];
		mean = variance = quadraticsum = stdev = 0;
		for (uint32_t i=0;i<samples.size();++i){
			mean += samples[i];
			quadraticsum += (double)((double)samples[i] * samples[i]);
			xmin = std::min(xmin, (double)samples[i]);
			xmax = std::max(xmax, (double)samples[i]);
		}
		mean /= samples.size();
		variance = quadraticsum/samples.size() - mean * mean;
		variance *= samples.size() / (samples.size() - 1.0);
		stdev = std::sqrt(variance);
		populations.clear();
		populations.resize(nDivision+1);
		xmedian.resize(nDivision+1);
		for (uint32_t i=0;i<samples.size();++i){
			double lgap = samples[i] - xmin;
			double rgap = xmax - samples[i];
			double part = (lgap) / (lgap + rgap);
			part *= nDivision;
			++populations[(int)part];
		}
		ymin = populations[0]; ymax = populations[0];
		for (int i=0;i<=nDivision;++i){
			double xVal = (xmin * i + xmax * (nDivision - i)) / nDivision;
			xmedian[i] = xVal;
			ymin = std::min(ymin, (double)populations[i]);
			ymax = std::max(ymax, (double)populations[i]);
		}
	}

	void plot(){
		plstream p;
		p.init();
		p.env(xmin,xmax,ymin,ymax,0,0);
		p.lab("(x)","(y)", "distribution");
		for (int i=0;i<(int)xmedian.size()-1;++i){
			p.join(xmedian[i],populations[i],xmedian[i+1],populations[i+1]);
		}

	}
	double mean;
	double quadraticsum;
	double variance;
	double stdev;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	int nDivision;
	std::vector<int> populations;
	std::vector<double> xmedian;
};

struct timer{
	timer(std::function<uint64_t()> func, uint64_t unit, uint64_t mult) : func(func), unit(unit), mult(mult)
	{
		initTime = func();
		times.resize(1000000);
	}
	void init(){
		initTime = func();
	}
	uint64_t elapsedns(){
		uint64_t currentTime = func();
		uint64_t delta = currentTime - initTime;
		return (delta * mult) / unit;
	}
	void settime(){
		times.push_back(elapsedns());
	}
	void clear(){
		times.clear();
		initTime = func();
	}
	std::function<uint64_t()> func;
	std::vector<uint64_t> times;
	uint64_t unit;
	uint64_t initTime;
	uint64_t mult;
};

extern inline unsigned long long __attribute__((always_inline)) rdtsc64(){
	unsigned int hi, lo;
	__asm__ __volatile__(
			"xorl %%eax, %%eax\n\t"
			"cpuid\n\t"
			"rdtsc"
			: "=a"(lo), "=d"(hi)
			  : /* no inputs */
			  : "rbx", "rcx");
	return ((unsigned long long)hi << 32ull) | (unsigned long long)lo;
}

extern inline timespec gettimefunc(){
	timespec ts;
	//clock_gettime(CLOCK_MONOTONIC, &ts);
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts;
}

extern inline uint64_t gettime64(){
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ((uint64_t)ts.tv_sec * 1000000000) + ts.tv_nsec;
}


inline void clocktest(){
	printf("CPU Clock : %lld HZ\n",cpufreq() );
	std::function<uint64_t()> rdtscfnc = [](){ return rdtsc64(); };
	std::function<uint64_t()> gtfnc = [](){
		return gettime64();
	};
	timer rdtsctimer(rdtscfnc, cpufreq(), 1000000000);
	timer tftimer(gtfnc, 1, 1);

	/*
	for (int i=0;i<1000;++i){
		rdtsctimer.settime();
		tftimer.settime();
	}

	printf("timer consistency test\n");

	for (int i=0;i<1000;++i){
		printf("rdtsc %lld ns\n",rdtsctimer.times[i]);
		printf("gett  %lld ns\n",tftimer.times[i]);
	}*/

	int nSpeedTest = 100000;


	printf("timer speed test(function call only)\n");
	rdtsctimer.clear();
	for (int i=0;i<nSpeedTest;++i){
		rdtsc64();
	}
	rdtsctimer.settime();
	printf("%d times rdtsc : %lld ns\n", nSpeedTest, rdtsctimer.times.back());

	//printf("timer speed test\n");
	rdtsctimer.clear();
	timespec ts;
	for (int i=0;i<nSpeedTest;++i){
		clock_gettime(CLOCK_REALTIME, &ts);
		//gettimefunc();
	}
	rdtsctimer.settime();
	printf("%d times gett  : %lld ns\n", nSpeedTest, rdtsctimer.times.back());


	printf("timer speed test(entire timer call)\n");
	rdtsctimer.clear();
	for (int i=0;i<nSpeedTest;++i){
		rdtsctimer.settime();
	}
	printf("%d times rdtsc : %lld ns\n", nSpeedTest, rdtsctimer.times.back());
	statistics rdtstat(rdtsctimer.times);
	printf("mean : %lf stdev : %lf ns\n", rdtstat.mean, rdtstat.stdev);

	tftimer.clear();
	for (int i=0;i<nSpeedTest;++i)
		tftimer.settime();
	printf("%d times gett  : %lld ns\n", nSpeedTest, tftimer.times.back());
	statistics gettstat(tftimer.times);
	printf("mean : %lf stdev : %lf ns\n", gettstat.mean, gettstat.stdev);

	rdtstat.plot();
	//gettstat.plot();
}





#endif /* CLOCKTEST_HPP_ */
