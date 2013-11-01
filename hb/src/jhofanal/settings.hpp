#ifndef __GXX_EXPERIMENTAL_CXX0X__
#define _USE_BOOST_
#endif

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
#include <map>
#include <set>
#include <assert.h>
#include <algorithm>
using namespace std;

typedef vector<double> vd;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<string> vs;
typedef pair<int, int> pi;
typedef vector<vd> vvd;
typedef vector<vvd> v3d;
typedef vector<pi> vpi;
typedef pair<vi, vi> pvi;
typedef vector<pvi> vpvi;