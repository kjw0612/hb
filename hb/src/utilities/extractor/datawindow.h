#ifndef readingwindow_h__
#define readingwindow_h__
#include <deque>
#include <map>
#include <algorithm>
#include "basereaders.h"

struct DataWindow{
	struct Book{
		Book(int maxSize = 50000) : maxSize(maxSize), lastprice(0.) {}
		void push(double val, int timestamp, int isTraded = 0){
			if (isTraded){
				lastprice = val;
			}
			vals.push_back(val);
			timestamps.push_back(timestamp);
			lastprices.push_back(lastprice);
			if ((int)vals.size() > maxSize){
				vals.pop_front();
				timestamps.pop_front();
				lastprices.pop_front();
			}
		}
		int maxSize;
		double lastprice; // technically this member name is untrue. Rather, naming like "last traded price" would be correct.
		std::deque<double> vals, lastprices;	
		std::deque<int> timestamps;
	};
	void clear(){
		bookmap.clear();
	}
	void push(const std::string& code, double val, int timestamp, int isTraded = 0){
		std::string d_code = code;
		std::transform(d_code.begin(),d_code.end(),d_code.begin(),::toupper);
		if (bookmap.find(d_code) == bookmap.end()){
			bookmap[d_code] = Book();
		}
		bookmap[d_code].push(val, timestamp, isTraded);
	}
	std::map<std::string, Book> bookmap;
};


#endif // readingwindow_h__
