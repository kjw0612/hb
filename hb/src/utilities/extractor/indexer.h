#ifndef indexer_h__
#define indexer_h__

#include "datawindow.h"
#include "descset.h"
#include "basereaders.h"
#include "range_query_tree.h"
#include "packethandler.h"

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
		MINTIME = 8000000, MAXTIME = 16000000, BUCKET_UNIT = 10
	};
	
	range_query_tree<long long> rtllmin, rtllmax;

	void setIndexTree()
	{
		rtllmin = range_query_tree<long long> (0,(MAXTIME-MINTIME)/BUCKET_UNIT,std::min<long long>, 50000000000LL);
		rtllmax = range_query_tree<long long> (0,(MAXTIME-MINTIME)/BUCKET_UNIT,std::max<long long>, -1LL);
		FILE *fpidx;
		fopen_s(&fpidx,(filepath + ".index").c_str(),"rt");
		while(!feof(fpidx)){
			int ts;
			long long minOffset, maxOffset;
			fscanf_s(fpidx,"%d %lld %lld",&ts,&minOffset,&maxOffset);
			rtllmin.update((ts-MINTIME)/BUCKET_UNIT,minOffset);
			rtllmax.update((ts-MINTIME)/BUCKET_UNIT,maxOffset);
		}
		fclose(fpidx);
	}

	inline std::pair<long long, long long> get_interval_within(int a_time, int b_time){
		long long l = rtllmin.read((a_time-MINTIME)/BUCKET_UNIT, (b_time-MINTIME)/BUCKET_UNIT);
		long long r = rtllmax.read((a_time-MINTIME)/BUCKET_UNIT, (b_time-MINTIME)/BUCKET_UNIT);
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
		std::vector<long long> minTimeOffset((MAXTIME-MINTIME+100)/BUCKET_UNIT,-1), maxTimeOffset((MAXTIME-MINTIME+100)/BUCKET_UNIT,-1);
		PacketSubject psb(filepath, type);
		PacketHandler hdlr;

		psb.push(&hdlr);
		int c = 0;
		int lastTimeStamp = 0;
		while(psb.next()){
			if (psb.rdr->castedRawType == t_KrxOptionsPolling)
				continue;
			if (psb.rdr->castedRawType == t_KrxOptionsGreek){
				if (hdlr.impl->timestampi >= 16000000)
					continue;
			}
			if (hdlr.impl->timestampi != 0){
				if (lastTimeStamp - hdlr.impl->timestampi > 6000000)
				{	break; }
				lastTimeStamp = std::max(lastTimeStamp, hdlr.impl->timestampi);

				int tstmpbuck = (hdlr.impl->timestampi-MINTIME)/BUCKET_UNIT;
				long long poffset = psb.rdr->rd.prevoffset;
				if (minTimeOffset[tstmpbuck]==-1){
					minTimeOffset[tstmpbuck] = poffset;
					maxTimeOffset[tstmpbuck] = poffset;
				}
				else{
					minTimeOffset[tstmpbuck] = std::min(minTimeOffset[tstmpbuck],poffset);
					maxTimeOffset[tstmpbuck] = std::min(maxTimeOffset[tstmpbuck],poffset);
				}
			}
		}

		FILE *fo;
		fopen_s(&fo,(filepath + ".index").c_str(),"wt");

		for (int i=0;i<(int)minTimeOffset.size();++i){
			if (minTimeOffset[i] != -1){
				fprintf(fo,"%d %lld %lld\n",(i*BUCKET_UNIT+MINTIME),minTimeOffset[i],maxTimeOffset[i]);
			}
		}
		fclose(fo);

		setIndexTree();
	}
	std::string filepath;
	char type;
};

#endif // indexer_h__
