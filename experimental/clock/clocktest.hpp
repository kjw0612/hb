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

struct timer{
	timer(std::function<uint64_t()> func, uint64_t unit, uint64_t mult) : func(func), unit(unit), mult(mult)
	{
		initTime = func();
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
	return (ts.tv_sec * 1000000000) + ts.tv_nsec;
}


inline void clocktest(){
	printf("CPU Clock : %lld HZ\n",cpufreq() );
	std::function<uint64_t()> rdtscfnc = [](){ return rdtsc64(); };
	std::function<uint64_t()> gtfnc = [](){
		return gettime64();
	};
	timer rdtsctimer(rdtscfnc, cpufreq(), 1000000000);
	timer tftimer(gtfnc, 1, 1);
	for (int i=0;i<1000;++i){
		rdtsctimer.settime();
		tftimer.settime();
	}

	printf("timer consistency test\n");

	for (int i=0;i<1000;++i){
		printf("rdtsc %lld ns\n",rdtsctimer.times[i]);
		printf("gett  %lld ns\n",tftimer.times[i]);
	}

	int nSpeedTest = 100000;
	/*
	printf("timer speed test\n");
	rdtsctimer.clear();
	for (int i=0;i<nSpeedTest;++i){
		rdtsctimer.settime();
	}
	printf("%d times rdtsc : %lld ns\n", nSpeedTest, rdtsctimer.times.back());
	tftimer.clear();
	for (int i=0;i<nSpeedTest;++i)
		tftimer.settime();
	printf("%d times gett  : %lld ns\n", nSpeedTest, tftimer.times.back());
	*/

	printf("timer speed test\n");
	rdtsctimer.clear();
	for (int i=0;i<nSpeedTest;++i){
		rdtsc64();
	}
	rdtsctimer.settime();
	printf("%d times rdtsc : %lld ns\n", nSpeedTest, rdtsctimer.times.back());

	printf("timer speed test\n");
	rdtsctimer.clear();
	timespec ts;
	for (int i=0;i<nSpeedTest;++i){
		clock_gettime(CLOCK_REALTIME, &ts);
		//gettimefunc();
	}
	rdtsctimer.settime();
	printf("%d times gett  : %lld ns\n", nSpeedTest, rdtsctimer.times.back());
}




#endif /* CLOCKTEST_HPP_ */
