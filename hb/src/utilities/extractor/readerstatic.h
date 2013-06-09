#ifndef READERSTATIC_H
#define READERSTATIC_H

#include "singleton.h"
#include "readerset.h"
#include "brickbase.h"
#include <fstream>

class ReaderStatic : public Singleton<ReaderStatic> {
public:
	enum DataMask{
		FUT = 1,
		CALL = 2,
		PUT = 4,
		GREEK = 8,
	};

	ReaderStatic() : tempfile_(NULL) {}

	void setupPath_1(const std::string& synconfig_filename = "synconfig.txt"){
		std::ifstream fi(synconfig_filename.c_str());
		std::string a,b;
		while(!fi.eof()){
			fi >> a;
			std::getline(fi,b);
			while(b[0]==32){
				b = b.substr(1);
			}
			if (!_strcmpi(a.c_str(),"basepath")){
				basepath = b; }
			if (!_strcmpi(a.c_str(),"fut")){
				futPath = b; }
			if (!_strcmpi(a.c_str(),"call")){
				callPath = b; }
			if (!_strcmpi(a.c_str(),"put")){
				putPath = b; }
			if (!_strcmpi(a.c_str(),"futDesc")){
				futDescPath = b; }
			if (!_strcmpi(a.c_str(),"optDesc")){
				optDescPath = b; }
			if (!_strcmpi(a.c_str(),"greek")){
				greekPath = b; }
		}
	}

	void setupDesc_2(){
		MakeDesc fmd(basepath+futDescPath,'f');
		MakeDesc omd(basepath+optDescPath,'o');
		ds_.append(fmd.getDS());
		ds_.append(omd.getDS());
	}

	void setupIndex_3(){
		fut_ = new ReaderSet (basepath+futPath,'f');
		call_ = new ReaderSet (basepath+callPath,'c');
		put_ = new ReaderSet (basepath+putPath,'p');
		greeks_ = new ReaderSet (basepath+greekPath,'o');
	}

	void setupDataTime_4(int start_time, int end_time, int dmsk = (FUT | CALL | PUT | GREEK)){
		if (dmsk | GREEK){
			greeksbs_ = greeks_->blrd.readBlockTime(start_time, end_time, BlockReader::GREEK);
			greeksbase_ = BrickBase(greeksbs_);
		}
		if (dmsk | FUT){
			futbs_ = fut_->blrd.readBlockTime(start_time, end_time, BlockReader::ORDERBOOK);
			futbase_ = BrickBase(futbs_);
		}
		if (dmsk | CALL){
			callbs_ = call_->blrd.readBlockTime(start_time, end_time, BlockReader::ORDERBOOK);
			callbase_ = BrickBase(callbs_);
		}
		if (dmsk | PUT){
			putbs_ = put_->blrd.readBlockTime(start_time, end_time, BlockReader::ORDERBOOK);
			putbase_ = BrickBase(putbs_);
		}
	}

	ReaderSet & fut(){
		return *fut_;
	}
	ReaderSet & call(){
		return *call_;
	}
	ReaderSet & put(){
		return *put_;
	}
	ReaderSet & greeks(){
		return *greeks_;
	}

	std::vector<Brick *>& futbs(){
		return futbs_;
	}
	std::vector<Brick *>& callbs(){
		return callbs_;
	}
	std::vector<Brick *>& putbs(){
		return putbs_;
	}
	std::vector<Brick *>& greeksbs(){
		return greeksbs_;
	}

	BrickBase & futbase(){
		return futbase_;
	}
	BrickBase & callbase(){
		return callbase_;
	}
	BrickBase & putbase(){
		return putbase_;
	}
	BrickBase & greeksbase(){
		return greeksbase_;
	}
	FILE *& tempfile(){
		return tempfile_;
	}
	DescSet & ds(){
		return ds_;
	}

private:
	DescSet ds_;
	ReaderSet *fut_, *call_, *put_, *greeks_;
	std::vector<Brick *> futbs_, callbs_, putbs_, greeksbs_;
	std::string basepath, futPath, callPath, putPath, greekPath, futDescPath, optDescPath;
	BrickBase futbase_, callbase_, putbase_, greeksbase_;
	FILE * tempfile_;
};


#endif // !READERSTATIC_H
