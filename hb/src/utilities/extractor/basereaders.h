#ifndef BASE_READERS_H
#define BASE_READERS_H

#include <vector>
#include "rawdatareader.hpp"

bool isQuotationType(long long castedRawType){
	switch(castedRawType)
	{
	case t_KrxFuturesTradeBestQuotation:
	case t_KrxOptionsTradeBestQuotation:
	case t_KrxFuturesTrade:
	case t_KrxOptionsTrade:
	case t_KrxFuturesBestQuotation:
	case t_KrxOptionsBestQuotation:
		return true;
	default:;
		return false;
	}
}

class DataReader{
public:
	DataReader(){}
	DataReader(const std::string& filename) : filename(filename), rd(filename), timestamp(0) {}
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
	std::string filename;
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
		Impl() : timestampi(0)
		{
			optheader.m_rawmsg = NULL;
			futheader.m_rawmsg = NULL;
		}
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
	PacketHandler(Impl * a_impl) : impl(a_impl) {}
	~PacketHandler(){ delete impl; }
	void update(long long capturedType, char *msg){
		impl->update(capturedType, msg);
	}
	Impl * impl;
private:
	template<class U> friend class MakeHandlerImpl;
};

class PacketSubject{
public:
	PacketSubject(const std::string& filepath, char type)
	{
		switch(tolower(type))
		{
		case 'f':
			{
				frdr = new KospiFuturesReader(filepath);
				rdr = frdr; break;
			}
		case 'p': case 'c': case 'o':
			{
				ordr = new KospiOptionsReader(filepath);
				rdr = ordr; break;
			}
		}
	}

	~PacketSubject(){
		delete rdr;
	}

	void push(PacketHandler* hdlr){
		handler.push_back(hdlr);
	}

	bool next(){
		if (rdr->empty())
			return false;
		rdr->next();
		for (int i=0;i<(int)handler.size();++i){
			handler[i]->update(rdr->castedRawType, rdr->content);
		}
		return true;
	}

	KospiOptionsReader *ordr;
	KospiFuturesReader *frdr;
	DataReader *rdr;
	std::vector<PacketHandler *> handler;
};



#endif // !BASE_READERS_H
