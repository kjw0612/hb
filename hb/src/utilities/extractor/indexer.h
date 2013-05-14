#ifndef indexer_h__
#define indexer_h__

#include "readers.h"
#include "rawdatareader.hpp"

struct Indexer{
	Indexer(const std::string& filepath, const std::string& yyyymmdd)
	{
		RawDataReader rdr(filepath);
		while(rdr.next()){
		}
	}
};

#endif // indexer_h__
