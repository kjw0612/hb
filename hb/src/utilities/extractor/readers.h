#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "rawdatareader.hpp"

class DataReader{
public:
	DataReader(const std::string& filename) : rd(filename), timestamp(0){}
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
};

class KospiOptionsReader : public DataReader{
public:
	KospiOptionsReader(const std::string& filename) : DataReader(filename) {}
	void setType(){
		optionsType = getTypeKrxOptionsHeader(content);
	}
	eKrxOptionsHeader optionsType;
};

class KospiFuturesReader : public DataReader{
public:
	KospiFuturesReader(const std::string& filename) : DataReader(filename) {}
	void setType(){
		futuresType = getTypeKrxFuturesHeader(content);
	}
	eKrxFuturesHeader futuresType;
};
