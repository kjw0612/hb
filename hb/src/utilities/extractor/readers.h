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

	Brick(const char *src, int size, long long castedRawType, int timestamp = -1){
		this->size = size; this->timestamp = timestamp; this->castedRawType = castedRawType;
		content = new char[size+1];
		memcpy(content, src, size);
		content[size] = 0;
	}

	~Brick(){
		delete [] content;
	}
};

template <class T>
class BlockReader{
public:
	BlockReader(const std::string& filename, Indexer* indexer) : reader(filename), indexer(indexer) {}

	void clearbricks(){
		for (int i=0;i<(int)bricks.size();++i){
			delete bricks[i];
		}
		bricks.clear();
	}

	const std::vector<Brick *>& readBlockTime(int a_time, int b_time){
		std::pair<long long, long long> l_r = indexer->get_interval_within(a_time,b_time);
		RawDataReader& rrd = reader.rd;
		rrd.seek(l_r.first);
		while(reader.next() && rrd.prevoffset <= l_r.second)
		{
			if (isQuotationType(reader.castedRawType)){
				bricks.push_back(new Brick(rrd.msg, rrd.sz, reader.castedRawType, reader.timestamp));
			}
		}
		return bricks;
	}

	std::vector<Brick *> bricks;
	T reader;
	Indexer* indexer;
};


template <class T>
class ReaderSet{
public:
	ReaderSet(const std::string& filename, char type)
		: indexer(filename, type), blrd(filename, &indexer)
	{	
		indexer.run();
		indexer.setIndexTree();
	}

	Indexer indexer;
	BlockReader<T> blrd;
};

class PriceCaptureImpl : public PacketHandler::Impl{
public:
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

template<class IMPL_TYPE>
class MakeHandlerImpl{
public:
	operator PacketHandler(){
		return PacketHandler(new IMPL_TYPE());
	}
};


#endif // readers_h__
