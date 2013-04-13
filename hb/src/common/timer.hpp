/*
 * timer.hpp
 *
 *  Created on: 2013. 4. 13.
 *      Author: koval
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

namespace hb{


	// couldn't think single reason why we should use other timers instead of rdtsc64();
	inline unsigned long long __attribute__((always_inline)) rdtsc64(){
		uint64_t res;
		asm("cpuid");
		asm volatile("rdtsc" : "=A" (res));
		return res;
	}

	const unsigned long long VmCpuFreq = 2393244000LL; // cat /proc/cpuinfo

	class Timer{
	private:

		inline void init(){
			timestamp = rdtsc64();
		}
	public:
		enum{
			NullClockTime = -1,
		};

// below should be defined in another file. not here.
#define VM_SETTING
#ifdef VM_SETTING
#define CPU_DEF_FREQ VmCpuFreq
#endif

		Timer(unsigned long long __cpu_freq = CPU_DEF_FREQ, unsigned long long __init_clock_time = NullClockTime)
		: cpu_freq(__cpu_freq), init_clock_time(__init_clock_time)
		{
			init();
		}

		inline void reset(){
			prev_timestamp = timestamp;
			timestamp = rdtsc64();
		}

		inline unsigned long long elapsedts(){ // this means timestamp. not terra-seconds.
			reset();
			return timestamp - prev_timestamp;
		}

		inline unsigned long long elapsedns(){
			return elapsedts() * 1000000000 / cpu_freq;
		}

		void report(FILE *_out){
			fprintf(_out, "elapsed time : %lld(ns)\n", elapsedns());
		}

		unsigned long long timestamp;
		unsigned long long prev_timestamp;
		unsigned long long cpu_freq;
		unsigned long long init_clock_time;
	};
}


#endif /* TIMER_HPP_ */
