#ifndef indexer_h__
#define indexer_h__

#include "readers.h"
#include "rawdatareader.hpp"
#include "range_query_tree.h"

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
		: filepath(filepath), type(type)
	{}

	enum {
		MINTIME = 8000000, MAXTIME = 16000000
	};
	
	range_query_tree<long long> rtllmin, rtllmax;

	void setIndexTree()
	{
		rtllmin = range_query_tree<long long> (MINTIME,MAXTIME,std::min<long long>, 50000000000LL);
		rtllmax = range_query_tree<long long> (MINTIME,MAXTIME,std::max<long long>, -1LL);
		FILE *fpidx;
		fopen_s(&fpidx,(filepath + ".index").c_str(),"rt");
		while(!feof(fpidx)){
			int ts;
			long long minOffset, maxOffset;
			fscanf_s(fpidx,"%d %lld %lld",&ts,&minOffset,&maxOffset);
			rtllmin.update(ts,minOffset);
			rtllmax.update(ts,maxOffset);
		}
		fclose(fpidx);
	}

	inline std::pair<long long, long long> get_interval_within(int a_time, int b_time){
		long long l = rtllmin.read(a_time), r = rtllmax.read(b_time);
		return std::make_pair(l,r);
	}

	void getQuestionTest()
	{
		int testvp[5][2] = {{8000000,15000000},{8500000,8600000},
		{9000000,10000000},{12000000,15000000},{13000000,16000000}};
		for (int i=0;i<5;++i){
			int a,b;
			a = testvp[i][0]; b = testvp[i][1];
			std::pair<long long, long long> l_r = get_interval_within(a,b);
			printf("testval[%d,%d] = %lld %lld\n",a,b,l_r.first,l_r.second);
		}
	}

	void run()
	{
		FILE *fotest;
		fopen_s(&fotest,(filepath + ".index").c_str(),"rt");
		if (fotest != NULL){
			fclose(fotest);
			return;
		}
		std::vector<long long> minTimeOffset(25000000,-1), maxTimeOffset(25000000,-1);
		PacketSubject psb(filepath, type);
		PacketHandler hdlr;

		psb.push(&hdlr);
		int c = 0;
		int lastTimeStamp = 0;
		while(psb.next()){
			if (hdlr.impl->timestampi != 0){
				if (lastTimeStamp - hdlr.impl->timestampi > 6000000){
					break;
				}
				lastTimeStamp = std::max(lastTimeStamp, hdlr.impl->timestampi);
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

		/*
		std::vector<long long> minminOffset(25000000,-1);
		long long curMin = -1;
		
		for (int i=(int)minTimeOffset.size()-1;i>=0;--i){
			minminOffset[i] = minTimeOffset[i];
			if (curMin != -1 && minminOffset[i] != -1){
				minminOffset[i] = std::min(minminOffset[i],curMin);
				curMin = minminOffset[i];
			}
			if (curMin == -1){
				curMin = minminOffset[i];
			}
		}*/

		FILE *fo;
		fopen_s(&fo,(filepath + ".index").c_str(),"wt");

		for (int i=0;i<(int)minTimeOffset.size();++i){
			if (minTimeOffset[i] != -1){
				fprintf(fo,"%d %lld %lld\n",i,minTimeOffset[i],maxTimeOffset[i]);
			}
		}
		fclose(fo);

		setIndexTree();
	}
	std::string filepath;
	char type;
};

#endif // indexer_h__
