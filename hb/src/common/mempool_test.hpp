#ifndef mempool_test_hpp
#define mempool_test_hpp

#include "mempool.hpp"
#include <math.h>

TEST (MemPoolTest, EmptyTest){
	EXPECT_EQ(4.0, std::sqrt(16.0));
	EXPECT_EQ(25.4, std::sqrt(645.16));
	EXPECT_EQ(50.332, std::sqrt(2533.310224));
}


#endif
