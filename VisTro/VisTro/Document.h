#pragma once
class Document;

#include "MarketDataLine.h"
#include <string>
#include <vector>

class Document
{
public:
	enum Status { 
		NONE,
		LOADING,
		LOAD_FAIL,
		SHOW,
	};

	Document(const char * filepath);
	~Document(void);

	Status status;
	std::string filepath;

	std::vector<MarketDataLine *> mdls;
	int upperBound(xTime time);

	double LoadingPhase;
};

