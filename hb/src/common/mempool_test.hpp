#ifndef mempool_test_hpp
#define mempool_test_hpp

#include "mempool.hpp"
#include "timer.hpp"
#include <math.h>
#include <functional>
#include <vector>
#include <memory>
#include <stdlib.h>

namespace testset{
	using namespace hb;
	using namespace std;

	void mempool_listener_test(){
	}

	void mem_alloc_same_large(function<char *(int )> allocfnc, function<void(char *)> freefnc,
			int ntimes, int chunk_size, int fragmentation)
	{
		char* res[1024];
		Timer timer;
		printf("function test - evaluating performance\n");
		printf("over allocation ");		printf("and deletion ");
		printf("for allocation times %d, ", ntimes);
		if (fragmentation){
			printf("(incremental) chunk size starts from %d\n", chunk_size);
		}
		else{
			printf("(equal) chunk size %d\n", chunk_size);
		}
		int n = 0;
		if (fragmentation){
			char *front = allocfnc(chunk_size);
			char *back = allocfnc(chunk_size);
			for (int i=0;i<ntimes;++i){
				char *newback = allocfnc(chunk_size+i);
				freefnc(back);
				back = newback;
			}
			freefnc(front);
		}
		else{
			srand(600);
			for (int i=0;i<1024;i++) res[i] = NULL;
			for (int i=0;i<ntimes;++i){
				int r = rand()%1024;
				if (res[r] != NULL) freefnc(res[r]);
				res[r] = allocfnc(chunk_size);
			}
			for (int i=0;i<1024;i++) if (res[i] != NULL) freefnc(res[i]);
		}
		timer.report(stdout);
	}

	auto_ptr<MemPool> mp;
	MemPool * mp_ptr;

	void mempool_performance_test(int fragmentation = 0){
		Timer timer;
		mp = auto_ptr<MemPool> (new MemPool(524288, 5));
		mp_ptr = mp.get();
		printf("---------------------MemPoolTime--------------\n");
		if (fragmentation==1){
			printf("trying to make a fragmentation");
		}
		printf("mempool allocation time - ");
		timer.report(stdout);
		//function<char *(int )> mpallocfnc = [](int size) -> char *{ return mp.readyBlock(size); };
		auto mpallocfnc = [](int size) -> char *{ mp_ptr->doneBlock(); return mp_ptr->readyBlock(size); };
		auto mpfreefnc = [](char *ptr) -> void { /* do nothing */ };
		mem_alloc_same_large(mpallocfnc, mpfreefnc, 131072, 512, fragmentation);
		printf("---------------------MemPoolTimeEnd-----------\n");
		printf("---------------------New[]Time----------------\n");
		timer.reset();
		auto newfnc = [](int size) -> char *{ return new char[size]; };
		auto deletefnc = [](char *ptr) -> void { delete ptr; };
		mem_alloc_same_large(newfnc, deletefnc, 131072, 512, fragmentation);
		printf("---------------------New[]TimeEnd-------------\n");
		mp.release();

	}

	TEST (MemPoolTest, EmptyTest){
		mempool_listener_test();
		mempool_performance_test(0);
		//mempool_performance_test(1);
		//EXPECT_EQ(4.0, std::sqrt(16.0));
		//EXPECT_EQ(25.4, std::sqrt(645.16));
		//EXPECT_EQ(50.332, std::sqrt(2533.310224));
	}
}


#endif
