#ifndef readers_h__
#define readers_h__

#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "indexer.h"
#include "macros.h"
#include "singleton.h"
#include "basereaders.h"
#include <vector>

struct Brick : private Uncopyable{
	char * content;
	int size, timestamp;
	long long castedRawType;
	double askprices[5], bidprices[5];
	double currentprice;

	Brick(const char *src, int size, long long castedRawType, int timestamp = -1){
		this->size = size; this->timestamp = timestamp; this->castedRawType = castedRawType;
		content = new char[size+1];
		memcpy(content, src, size);
		content[size] = 0;
	}

	Brick(const char *src, int size, long long castedRawType, double askprices[5], double bidprices[5], double currentprice, int timestamp = -1){
		this->size = size; this->timestamp = timestamp; this->castedRawType = castedRawType;
		memcpy(this->askprices,askprices,sizeof(this->askprices));
		memcpy(this->bidprices,bidprices,sizeof(this->bidprices));
		this->currentprice = currentprice;
		content = new char[size+1];
		memcpy(content, src, size);
		content[size] = 0;
	}

	~Brick(){
		delete [] content;
	}
};


class PriceCaptureImpl : public PacketHandler::Impl{
public:
	PriceCaptureImpl() : PacketHandler::Impl()
	{
		memset(askprices,0,sizeof(askprices));
		memset(bidprices,0,sizeof(bidprices));
		currentprice = 0;
	}

	double askprices[5], bidprices[5];
	double currentprice;

	template <class some_packet_type>
	void setLimitOrderQuotes(const some_packet_type *header){
		askprices[0] = ATOI_LEN(header->ask1price) / 100.0;
		askprices[1] = ATOI_LEN(header->ask2price) / 100.0;
		askprices[2] = ATOI_LEN(header->ask3price) / 100.0;
		askprices[3] = ATOI_LEN(header->ask4price) / 100.0;
		askprices[4] = ATOI_LEN(header->ask5price) / 100.0;
		bidprices[0] = ATOI_LEN(header->bid1price) / 100.0;
		bidprices[1] = ATOI_LEN(header->bid2price) / 100.0;
		bidprices[2] = ATOI_LEN(header->bid3price) / 100.0;
		bidprices[3] = ATOI_LEN(header->bid4price) / 100.0;
		bidprices[4] = ATOI_LEN(header->bid5price) / 100.0;
	}

	void update(long long capturedType, char *msg){
		PacketHandler::Impl::update(capturedType,msg);
		switch(capturedType){
			case t_KrxFuturesTradeBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesTradeBestQuotation);
				break;
			case t_KrxOptionsTradeBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsTradeBestQuotation);
				break;
			case t_KrxFuturesTrade:
				currentprice = ATOI_LEN(futheader.m_KrxFuturesTrade->currentprice) / 100.0;
				break;
			case t_KrxOptionsTrade:
				currentprice = ATOI_LEN(optheader.m_KrxOptionsTrade->currentprice) / 100.0;
				break;
			case t_KrxFuturesBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesBestQuotation);
				break;
			case t_KrxOptionsBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsBestQuotation);
				break;
			default:;
		}
	}
};


class BlockReader{
public:
	BlockReader(const std::string& filename, char type, Indexer* indexer) : psbj(filename, type), indexer(indexer) {}

	void clearbricks(){
		for (int i=0;i<(int)bricks.size();++i){
			delete bricks[i];
		}
		bricks.clear();
	}

	const std::vector<Brick *>& readBlockTime(int a_time, int b_time){
		std::pair<long long, long long> l_r = indexer->get_interval_within(a_time,b_time);
		DataReader* rd = psbj.rdr;
		RawDataReader& rrd = psbj.rdr->rd;
		PriceCaptureImpl* pcapimpl = new PriceCaptureImpl();
		PacketHandler pcaphd(pcapimpl);
		psbj.push(&pcaphd);
		rrd.seek(l_r.first);
		while(psbj.next() && rrd.prevoffset <= l_r.second)
		{
			if (isQuotationType(rd->castedRawType)){
				bricks.push_back(new Brick(rrd.msg, rrd.sz, rd->castedRawType,
					pcapimpl->askprices, pcapimpl->bidprices, pcapimpl->currentprice, pcapimpl->timestampi));
			}
		}
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


#endif // readers_h__
