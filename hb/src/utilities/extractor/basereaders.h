#ifndef BASE_READERS_H
#define BASE_READERS_H

#include <vector>
#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "rawdatareader.hpp"

inline bool isQuotationType(long long castedRawType){
	switch(castedRawType)
	{
	case t_KrxFuturesTradeBestQuotation:
	case t_KrxOptionsTradeBestQuotation:
	case t_KrxFuturesTrade:
	case t_KrxOptionsTrade:
	case t_KrxFuturesBestQuotation:
	case t_KrxOptionsBestQuotation:
	case t_KrxOptionsGreek:
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
		rdtscStamp = rd.rt;
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
	long long rdtscStamp;
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

struct Orderbook{
	Orderbook() {}
	Orderbook(double *_askprices, double *_bidprices, double currentprice, double expectedprice)
		: currentprice(currentprice), expectedprice(expectedprice)
	{
		memcpy(askprices,_askprices,sizeof(askprices));
		memcpy(bidprices,_bidprices,sizeof(bidprices));
	}

	int askprices[5], bidprices[5];
	int askquantities[5], bidquantities[5];
	double currentprice;
	double expectedprice;
};

struct Greeks{
	double delta, theta, vega, gamma , rho;
};

struct PacketInfo{
	PacketInfo(){}
	PacketInfo(const std::string& krcodestr, char *src, int size, long long castedRawType, int timestamp, long long rdtscStamp)
		: krcodestr(krcodestr), size(size), castedRawType(castedRawType), timestamp(timestamp), src(src), rdtscStamp(rdtscStamp) {}

	std::string krcodestr;
	char *src;
	int size;
	long long castedRawType;
	int timestamp;
	long long rdtscStamp;
};


struct Brick{
	enum Type{
		OrderBook,
		OrderBookGreeks,
		Greek,
	};
	enum GetDataType{
		MidPrice,
		WeightedMidPrice,
		Bid1,
		Ask1,
		Delta,
	};

	std::string krcode;

	void setKrCode(const std::string& krcode){
		this->krcode = krcode;
	}

	PacketInfo pi;
	Orderbook ob;
	Greeks grk;
	Type type;

	Brick() {}
	Brick(const PacketInfo& pi) : pi(pi) {}
	Brick(const PacketInfo& pi, const Orderbook& ob)
		: pi(pi), ob(ob), type(OrderBook) {}
	Brick(const PacketInfo& pi, const Orderbook& ob, const Greeks& grk)
		: pi(pi), ob(ob), grk(grk), type(OrderBookGreeks) {}
	Brick(const PacketInfo& pi, const Greeks& grk)
		: pi(pi), grk(grk), type(Greek) {}

	bool operator<(const Brick& rhs) const {
		return this->pi.timestamp < rhs.pi.timestamp;
	}

	double getValue(GetDataType dm) const {
		switch(dm){
			case MidPrice:
				return midPriceSimpleAvg();
			case WeightedMidPrice:
				return midPriceWeighted();
			case Bid1:
				return ob.bidprices[0];
			case Ask1:
				return ob.askprices[0];
			case Delta:
				return grk.delta;
		}
		return -100.0; // why -100? sorry. not any intention.
	}

	bool hollowMidPrice() const{
		return (ob.askprices[0] - ob.bidprices[0]) > ((ob.bidprices[0] - ob.bidprices[1]) * 1.5);
		// why 1.5? zz.
	}

	double midPriceSimpleAvg() const {
		return (ob.askprices[0] + ob.bidprices[0]) / 2;
	}

	double midPriceWeighted() const {
		return (ob.askprices[0] * ob.askquantities[0] + ob.bidprices[0] * ob.bidquantities[0])
			/ (double)(ob.askquantities[0]+ob.bidquantities[0]);
	}
};


#endif // !BASE_READERS_H
