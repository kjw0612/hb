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
	statistics(std::vector<uint64_t>& n_samples)
	: nDivision(50)
	{
		FILE *fo = fopen("output.txt","wt");
		for (int i=0;i<n_samples.size();++i){
			fprintf(fo,"%lld\n",n_samples[i]);
		}
		fclose(fo);
		return;
		std::vector<uint64_t> samples(n_samples.size()-1,0);

		for (uint32_t i=0;i<samples.size();++i){
			samples[i] = n_samples[i+1] - n_samples[i];
		}

		xmin = samples[0];
		xmax = samples[0];
		mean = variance = quadraticsum = stdev = 0;
		for (uint32_t i=0;i<samples.size();++i){
			mean += samples[i];
			quadraticsum += (double)((double)samples[i] * samples[i]);
			xmin = std::min(xmin, (double)samples[i]);
			xmax = std::max(xmax, (double)samples[i]);
		}
		//printf("[%lf,%lf]\n",xmin,xmax);
		xmin = xmax * 0.1;
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
		return;
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
		times.reserve(1000000);
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

extern inline uint64_t __attribute__((always_inline)) rdtsc64(){
	uint64_t ret;
		__asm__ __volatile__(
			"xorl %%eax, %%eax\n\t"
			"cpuid\n\t"
			"rdtsc"
			: "=A" (ret)
			  : "rbx", "rcx" );
	return ret;
	/*
	unsigned int hi, lo;
	__asm__ __volatile__(
			"xorl %%eax, %%eax\n\t"
			"cpuid\n\t"
			"rdtsc"
			: "=a"(lo), "=d"(hi)
			  : "rbx", "rcx");
	return ((unsigned long long)hi << 32ull) | (unsigned long long)lo;*/
	//return 0;


	uint64_t res;
	__asm__ volatile("rdtsc" : "=A" (res));
	return res;
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
	timer rdtsctimer(rdtscfnc, cpufreq(), 1000000000LL);
	timer tftimer(gtfnc, 1, 1);


	/*
	for (int i=0;i<100000;++i){
		rdtsctimer.settime();
		tftimer.settime();
	}

	printf("timer consistency test\n");

	for (int i=0;i<100000;++i){
		printf("rdtsc %lld ns\n",rdtsctimer.times[i]);
		printf("gett  %lld ns\n",tftimer.times[i]);
	}

	return;*/

	int nSpeedTest = 100000;

	int lo = 0;
	uint64_t res;


	printf("timer speed test(function call only)\n");
	rdtsctimer.clear();
	for (int i=0;i<nSpeedTest;++i){
		__asm__ volatile("rdtsc" : "=A" (res));
		//lo += res;
		/*
		__asm__ __volatile__(
				"xorl %%eax, %%eax\n\t"
				"cpuid\n\t"
				"rdtsc"
				: "=a"(lo), "=d"(hi)
				  : "rbx", "rcx");
				  */
		//rdtsc64();
	}

	printf("%d\n",lo);
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

	/*
	tftimer.clear();
	for (int i=0;i<nSpeedTest;++i)
		tftimer.settime();
	printf("%d times gett  : %lld ns\n", nSpeedTest, tftimer.times.back());
	statistics gettstat(tftimer.times);
	printf("mean : %lf stdev : %lf ns\n", gettstat.mean, gettstat.stdev);
	*/


	rdtstat.plot();
	//gettstat.plot();
}





#endif /* CLOCKTEST_HPP_ */
