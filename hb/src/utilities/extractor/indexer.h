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
		std::vector<long long> minTimeOffset(25000000,-1), maxTimeOffset(25000000,-1);
		PacketSubject psb(filepath, type);
		PacketHandler hdlr;

		psb.push(&hdlr);
		int c = 0;
		while(psb.next() && ++c < 1000000){
			if (hdlr.impl->timestampi != 0){
				long long poffset = psb.rdr->rd.prevoffset;
				if (minTimeOffset[hdlr.impl->timestampi]==-1){
					minTimeOffset[hdlr.impl->timestampi] = poffset;
					maxTimeOffset[hdlr.impl->timestampi] = poffset;
				}
				else{
					minTimeOffset[hdlr.impl->timestampi] = std::min(minTimeOffset[hdlr.impl->timestampi],poffset);
					maxTimeOffset[hdlr.impl->timestampi] = std::min(maxTimeOffset[hdlr.impl->timestampi],poffset);
				}
			}
			/*
			if (timestamps.back() < hdlr.impl->timestampi){
			}
			else if (timestamps.back() > hdlr.impl->timestampi)
				break;
				*/
		}

		FILE *fo = fopen((filepath + ".index").c_str(),"wt");

		for (int i=0;i<(int)minTimeOffset.size();++i){
			if (minTimeOffset[i] != -1){
				fprintf(fo,"%d %lld %lld\n",i,minTimeOffset[i],maxTimeOffset[i]);
			}
		}
		fclose(fo);
	}
	std::string filepath;
	char type;
};

#endif // indexer_h__
