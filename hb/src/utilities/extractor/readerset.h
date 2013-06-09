#ifndef readerset_h__
#define readerset_h__

#include "basereaders.h"
#include "indexer.h"

class BlockReader{
public:

	enum DataType{
		ORDERBOOK,
		GREEK
	};

	BlockReader(const std::string& filename, char type, Indexer* indexer) : psbj(filename, type), indexer(indexer) {}

	void clearbricks(){
		for (int i=0;i<(int)bricks.size();++i){
			delete bricks[i];
		}
		bricks.clear();
	}

	const std::vector<Brick *>& readBlockTime(int a_time, int b_time, DataType datatype){
		clearbricks();
		std::pair<long long, long long> l_r = indexer->get_interval_within(a_time,b_time);
		DataReader* rd = psbj.rdr;
		RawDataReader& rrd = psbj.rdr->rd;
		PriceCaptureImpl pcapimpl;
		PacketHandler pcaphd(&pcapimpl);
		psbj.push(&pcaphd);
		rrd.seek(l_r.first);
		printf("-");
		while(psbj.next() && rrd.prevoffset <= l_r.second)
		{
			if (isQuotationType(rd->castedRawType)){
				if (datatype == ORDERBOOK){
					bricks.push_back
						(new Brick(PacketInfo(pcapimpl.krcodestr, rrd.msg, rrd.sz, rd->castedRawType, pcapimpl.timestampi),*pcapimpl.ob));
				}
				else{
					bricks.push_back
						(new Brick(PacketInfo(pcapimpl.krcodestr, rrd.msg, rrd.sz, rd->castedRawType, pcapimpl.timestampi),*pcapimpl.grk));
				}
			}
		}
		printf("-");
		return bricks;
	}

	std::vector<Brick *> bricks;
	PacketSubject psbj;
	Indexer* indexer;
};


class ReaderSet{
public:
	ReaderSet(const std::string& filename, char type)
		: indexer(filename, type), blrd(filename, type, &indexer)
	{	
		indexer.run();
		indexer.setIndexTree();
	}

	Indexer indexer;
	BlockReader blrd;
};

#endif // readerset_h__
