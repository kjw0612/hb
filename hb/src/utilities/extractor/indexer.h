#ifndef indexer_h__
#define indexer_h__

#include "readers.h"
#include "rawdatareader.hpp"

/*

FILEPATH -> index\[f|o]yyyymmdd.txt
timestamp sti
9000001 100
9002000 50000
.
.
.
.

*/

struct Indexer{
	Indexer(const std::string& filepath, char type)
		: filepath(filepath), type(type) {}

	void run()
	{
		std::vector<int> timestamps;
		std::vector<long long> offsets;
		timestamps.reserve(10000000);
		offsets.reserve(10000000);
		PacketSubject psb(filepath, type);
		PacketHandler hdlr;
		psb.push(&hdlr);
		timestamps.push_back(0); offsets.push_back(0);
		int c = 0;
		while(psb.next() && ++c < 1000000){
			if (hdlr.impl->timestampi != 0){
				timestamps.push_back(hdlr.impl->timestampi);
				offsets.push_back(psb.rdr->rd.prevoffset);
			}
			/*
			if (timestamps.back() < hdlr.impl->timestampi){
			}
			else if (timestamps.back() > hdlr.impl->timestampi)
				break;
				*/
		}
	}
	std::string filepath;
	char type;
};

#endif // indexer_h__
