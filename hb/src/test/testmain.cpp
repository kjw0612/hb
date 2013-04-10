#include "gtest/gtest.h"
#include <cmath>
#include <test/testinclude.hpp>

double square_root(const double sqx){
	return sqrt(sqx);
}

TEST (SquareRootTest, PositiveNos){
	EXPECT_EQ(4.0, square_root(16.0));
	EXPECT_EQ(25.4, square_root(645.16));
	EXPECT_EQ(50.332, square_root(2533.310224));
}

TEST (SquareRootTest, ZeroAndNegativeNos){
	ASSERT_EQ(0.0, square_root(0.0));
	ASSERT_EQ(-1, square_root(-22.0));
}

int main(int argc, char ** argv){
	helloworld();
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
