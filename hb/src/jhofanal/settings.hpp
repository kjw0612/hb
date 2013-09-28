#ifdef _USE_BOOST_
#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>
using namespace boost;
#else
#include <memory>
#include <chrono>
#include <ctime>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
using namespace std;

typedef vector<double> vd;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<string> vs;
typedef pair<int, int> pi;
typedef vector<vd> vvd;
typedef vector<vvd> v3d;