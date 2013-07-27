#ifndef functional_h__
#define functional_h__
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

	inline static std::vector<long long> makegrid_ll(long long a, long long b, int nTimes){
		std::vector<long long> grid(nTimes);
		long long seq_i = 0;
		long long diff = (b-a);
		for (int i=0;i<nTimes;++i){
			grid[i] = a + ((long long)diff * (i)) / (nTimes-1);
		}
		return grid;
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

	template<class T>
	struct ptr_comp_func{
		typedef bool (*ptr_comp_func_type)(const T& a, const T &b);
		ptr_comp_func(ptr_comp_func_type comp) : comp(comp) {}
		bool operator ()(const T * const & a, const T * const & b) const {
			return comp(*a,*b);
		}
		ptr_comp_func_type comp;
	};
};


#endif // functional_h__
