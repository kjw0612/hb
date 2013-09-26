#ifdef __GXX_EXPERIMENTAL_CXX0X__ 
#include <shared_ptr>
#else
#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>
using namespace boost;
#endif

#include <vector>
#include <string>
#include <iostream>
using namespace std;

typedef vector<double> vd;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<string> vs;
typedef pair<int, int> pi;