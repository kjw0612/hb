#ifndef mempool_test_hpp
#define mempool_test_hpp

#include "mempool.hpp"
#include "timer.hpp"
#include <math.h>
#include <functional>
#include <vector>
#include <memory>

namespace testset{
	using namespace hb;
	using namespace std;

	void mempool_listener_test(){
	}

	void mem_alloc_same_large(function<char *(int )> allocfnc, function<void(char *)> freefnc,
			int ntimes, int chunk_size)
	{
		char* res;
		Timer timer;
		printf("function test - evaluating performance\n");
		printf("over allocation ");		printf("and deletion ");
		printf("for allocation times %d, (equal) chunk size %d\n", ntimes, chunk_size);
		int n = 0;
		for (int i=0;i<ntimes;i+=chunk_size){
			res = allocfnc(chunk_size);
			freefnc(res);
		}
		timer.report(stdout);
	}
	auto_ptr<MemPool> mp;
	MemPool * mp_ptr;

	void mempool_performance_test(){
		Timer timer;
		mp = auto_ptr<MemPool> (new MemPool(30000000, 5));
		mp_ptr = mp.get();
		printf("---------------------MemPoolTime--------------\n");
		printf("mempool allocation time - ");
		timer.report(stdout);
		//function<char *(int )> mpallocfnc = [](int size) -> char *{ return mp.readyBlock(size); };
		auto mpallocfnc = [](int size) -> char *{ mp_ptr->doneBlock(); return mp_ptr->readyBlock(size); };
		auto mpfreefnc = [](char *ptr) -> void { /* do nothing */ };
		mem_alloc_same_large(mpallocfnc, mpfreefnc, 1000000, 50000);
		printf("---------------------MemPoolTimeEnd-----------\n");
		printf("---------------------New[]Time----------------\n");
		timer.reset();
		auto newfnc = [](int size) -> char *{ return new char[size]; };
		auto deletefnc = [](char *ptr) -> void { delete ptr; };
		mem_alloc_same_large(newfnc, deletefnc, 1000000, 50000);
		printf("---------------------New[]TimeEnd-------------\n");

	}

	TEST (MemPoolTest, EmptyTest){
		mempool_listener_test();
		mempool_performance_test();
		//EXPECT_EQ(4.0, std::sqrt(16.0));
		//EXPECT_EQ(25.4, std::sqrt(645.16));
		//EXPECT_EQ(50.332, std::sqrt(2533.310224));
	}
}


#endif
