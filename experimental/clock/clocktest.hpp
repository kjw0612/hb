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
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts;
}


inline void clocktest(){
	printf("%lld\n", rdtsc64());
	printf("%ld\n",gettimefunc().tv_nsec);
	sleep(1);
	printf("%lld\n", rdtsc64());
	printf("%ld\n",gettimefunc().tv_nsec);
}




#endif /* CLOCKTEST_HPP_ */
