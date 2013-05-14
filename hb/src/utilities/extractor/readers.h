#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "rawdatareader.hpp"
#include "macros.h"
#include <vector>

class DataReader{
public:
	DataReader(){}
	DataReader(const std::string& filename) : rd(filename), timestamp(0) {}
	virtual void setType() = 0;
	bool next(){
		bool ret = rd.next();
		len = rd.sz;
		content = rd.msg;
		setType();
		return ret;
	}
	bool empty(){
		return rd.empty();
	}
	RawDataReader rd;
	char* content;
	int len, timestamp;
	long long castedRawType;
};

class KospiOptionsReader : public DataReader{
public:
	KospiOptionsReader() : DataReader() {} // NULL READER
	KospiOptionsReader(const std::string& filename) : DataReader(filename) {}
	void setType(){
		optionsType = getTypeKrxOptionsHeader(content);
		castedRawType = (long long)optionsType;
	}
	eKrxOptionsHeader optionsType;
};

class KospiFuturesReader : public DataReader{
public:
	KospiFuturesReader() : DataReader() {} // NULL READER
	KospiFuturesReader(const std::string& filename) : DataReader(filename) {}
	void setType(){
		futuresType = getTypeKrxFuturesHeader(content);
		castedRawType = (long long)futuresType;
	}
	eKrxFuturesHeader futuresType;
};

class PacketHandler{
public:
	class Impl{
	public:
		template <class some_packet_type>
		void setCodeTime(const some_packet_type *header){
			COPY_STR(krcode, header->krcode);
			timestampi = ATOI_LEN(header->timestamp);
		}
		virtual void update(long long capturedType, char *msg)
		{
			futheader.m_rawmsg = msg;
			optheader.m_rawmsg = msg;
			switch(capturedType)
			{
			case t_KrxFuturesTradeBestQuotation:
				setCodeTime(futheader.m_KrxFuturesTradeBestQuotation);
				break;
			case t_KrxOptionsTradeBestQuotation:
				setCodeTime(optheader.m_KrxOptionsTradeBestQuotation);
				break;
			case t_KrxFuturesTrade:
				setCodeTime(futheader.m_KrxFuturesTrade);
				break;
			case t_KrxOptionsTrade:
				setCodeTime(optheader.m_KrxOptionsTrade);
				break;
			case t_KrxFuturesBestQuotation:
				setCodeTime(futheader.m_KrxFuturesBestQuotation);
				break;
			case t_KrxOptionsBestQuotation:
				setCodeTime(optheader.m_KrxOptionsBestQuotation);
				break;
			default:;
			}
		}
		KrxOptionsHeader optheader;
		KrxFuturesHeader futheader;
		char krcode[20];
		int timestampi;
	};
	PacketHandler() : impl(new Impl()) {}
	~PacketHandler(){
		delete impl;
	}
	void update(long long capturedType, char *msg){
		impl->update(capturedType, msg);
	}
	Impl * impl;
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

class PacketSubject{
public:
	PacketSubject(const std::string& filename, char type)
	{
		switch(tolower(type))
		{
		case 'f':
			{ frdr = KospiFuturesReader(filename);
			rdr = &frdr; break; }
		case 'p': case 'c': case 'o':
			{ ordr = KospiOptionsReader(filename);
			rdr = &ordr; break; }
		}
	}

	void push(const PacketHandler& hdlr){
		handler.push_back(hdlr);
	}

	bool next(){
		if (rdr->empty())
			return false;
		rdr->next();
		for (int i=0;i<(int)handler.size();++i){
			handler[i].update(rdr->castedRawType, rdr->content);
		}
		return true;
	}

	KospiOptionsReader ordr;
	KospiFuturesReader frdr;
	DataReader *rdr;
	std::vector<PacketHandler> handler;
};

