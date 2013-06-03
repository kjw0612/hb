#ifndef readers_h__
#define readers_h__

#include "krx_kospi_futures.hpp"
#include "krx_kospi_options.hpp"
#include "indexer.h"
#include "macros.h"
#include "singleton.h"
#include "basereaders.h"
#include <vector>

struct Orderbook{
	Orderbook() {}
	Orderbook(double *_askprices, double *_bidprices, double currentprice, double expectedprice)
		: currentprice(currentprice), expectedprice(expectedprice)
	{
		memcpy(askprices,_askprices,sizeof(askprices));
		memcpy(bidprices,_bidprices,sizeof(bidprices));
	}

	double askprices[5], bidprices[5];
	double currentprice;
	double expectedprice;
};

struct Greeks{
	double delta, theta, vega, gamma , rho;
};

struct PacketInfo{
	PacketInfo(){}
	PacketInfo(const std::string& krcodestr, char *src, int size, long long castedRawType, int timestamp)
		: krcodestr(krcodestr), size(size), castedRawType(castedRawType), timestamp(timestamp), src(src) {}

	std::string krcodestr;
	char *src;
	int size;
	long long castedRawType;
	int timestamp;
};

class Functional{
public:
	static void timestampfunctest(){
		for (int i=9000000;i<12000000;i+=1221212){
			int x = Functional::timestamp2seq(i);
			int y = Functional::seq2timestamp(x);
			printf("%d %d %d\n",i,x,y);
		}
		for (int x = Functional::timestamp2seq(9000000); x <= Functional::timestamp2seq(16000000); x += 123456){
			int y = Functional::seq2timestamp(x);
			printf("%d %d\n",x,y);
		}
	}

	inline static int timestamp2seq(int timestamp){
		// 9000000 // HHMMSSxx
		const int ss_xx = 100, MM_ss = 60, HH_MM = 60;
		int seq = 0;
		seq += timestamp % 100; // xx
		timestamp /= 100; // HHMMSS
		seq += timestamp % 100 * ss_xx; // ss * 100
		timestamp /= 100; // HHMM
		seq += (timestamp % 100) * MM_ss * ss_xx; // MM * 60 * 100
		timestamp /= 100; // HH
		seq += timestamp * HH_MM * MM_ss * ss_xx;
		return seq;
	}

	inline static int seq2timestamp(int seq){
		const int ss_xx = 100, MM_ss = 60, HH_MM = 60;
		int HH = seq / (ss_xx * MM_ss * HH_MM);
		int mm = seq / (ss_xx * MM_ss) % HH_MM;
		int ss = seq / (ss_xx) % MM_ss;
		int xx = seq % ss_xx;
		return ((HH * 100 + mm) * 100 + ss) * 100 + xx;
	}

	inline static int tsdiff(int timestampa, int timestampb){
		int sqa = timestamp2seq(timestampa), sqb = timestamp2seq(timestampb);
		return sqa - sqb;
	}

	inline static std::vector<int> makegrid(int timestamp_st, int timestamp_end, int nTimes){
		std::vector<int> grid(nTimes);
		int seq_st = Functional::timestamp2seq(timestamp_st), seq_end = Functional::timestamp2seq(timestamp_end);
		for (int i=0;i<nTimes;++i){
			int seq_i = ((long long)seq_st * (nTimes-1-i) + (long long)seq_end * (i)) / (nTimes-1);
			int timestamp_i = Functional::seq2timestamp(seq_i);
			grid[i] = timestamp_i;
		}
		return grid;
	}
	template<class T>
	inline static bool ptr_comp(const T * const & a, const T * const & b)
	{
		return *a < *b;
	}
};

struct GreekBrick : private Uncopyable{
	PacketInfo pi;
	Greeks grk;
	GreekBrick(const PacketInfo& pi, const Greeks& grk) : pi(pi), grk(grk) {}
};

struct Brick : private Uncopyable{
	PacketInfo pi;
	Orderbook ob;

	Brick() {}
	Brick(const PacketInfo& pi) : pi(pi) {}
	Brick(const PacketInfo& pi, const Orderbook& ob) : pi(pi), ob(ob) {}

	bool operator<(const Brick& rhs) const {
		return this->pi.timestamp < rhs.pi.timestamp;
	}

	double midPriceSimpleAvg() const {
		return (ob.askprices[0] + ob.bidprices[0]) / 2;
	}
};


class Interpolation{
public:
	template<class x_type>
	static int locate(const std::vector<x_type>& xs, x_type x)
	{
		if (x < xs.front())
			return 0;
		else if (x > xs.back())
			return xs.end()-xs.begin()-2;
		else
			return std::upper_bound(xs.begin(),xs.end()-1,x)-xs.begin()-1;
	}

	template<class x_type>
	static int locate_ptr(const std::vector<x_type *>& xs, x_type * x)
	{
		if (*(x) < *(xs.front()))
			return 0;
		else if (*(xs.back()) < *(x))
			return xs.end()-xs.begin()-2;
		else
			return std::upper_bound(xs.begin(),xs.end()-1,x,Functional::ptr_comp<x_type>)-xs.begin()-1;
	}

	// xs are sorted in ascending order
	template<class x_type, class y_type>
	static y_type linear(const std::vector<x_type>& xs, const std::vector<y_type>& ys, x_type x)
	{
		int i = Interpolation::locate(xs, x);
		if (i==0 || i+1==xs.size() || xs[i] == x){
			return ys[i];
		}
		else{
			return (ys[i] * (x-xs[i]) + ys[i+1] * (xs[i+1] - x)) / (xs[i+1] - xs[i]);
		}
	}

	static double linear_bricks(const std::vector<Brick *>& bricks, int x, bool prev)
	{
		Brick brick; brick.pi.timestamp = x;
		int i = Interpolation::locate_ptr(bricks, &brick);
		if (i==0 || i+1==bricks.size() || bricks[i]->pi.timestamp == x){
			return bricks[i]->midPriceSimpleAvg();
		}
		else{
			if (prev){
				return bricks[i]->midPriceSimpleAvg();
			}
			else{
				return (bricks[i]->midPriceSimpleAvg() * Functional::tsdiff(x,bricks[i]->pi.timestamp) +
					bricks[i+1]->midPriceSimpleAvg() * Functional::tsdiff(bricks[i+1]->pi.timestamp,x))
					/ Functional::tsdiff(bricks[i+1]->pi.timestamp,bricks[i]->pi.timestamp);
			}
		}
	}
};

inline std::pair<std::vector<int>, std::vector<double> > bricks2MidPriceGrid
	(const std::vector<Brick *>& bricks, int mintime, int maxtime, int nTimes)
{
	std::vector<int> grids = Functional::makegrid(mintime, maxtime, nTimes);
	std::vector<double> midprices(nTimes);
	for (int i=0;i<(int)grids.size();++i){
		midprices[i] = Interpolation::linear_bricks(bricks, grids[i], true);
	}
	return std::make_pair(grids, midprices);
}

inline std::pair<std::vector<int>, std::vector<double> > aXpbY
	(double a, const std::pair<std::vector<int>, std::vector<double> >& X,
	double b, const std::pair<std::vector<int>, std::vector<double> >& Y)
{
	std::pair<std::vector<int>, std::vector<double> > ret;
	if (X.first.size()==Y.first.size() && X.second.size()==Y.second.size()){
		ret.first = X.first;
		ret.second = std::vector<double>(X.second.size());
		for (int i=0;i<(int)ret.second.size();++i){
			ret.second[i] = a*X.second[i] + b*Y.second[i];
		}
	}
	return ret;
}

inline std::pair<std::vector<int>, std::vector<double> > aXpb
(double a, const std::pair<std::vector<int>, std::vector<double> >& X,
 double b)
{
	std::pair<std::vector<int>, std::vector<double> > ret;
	ret.first = X.first;
	ret.second = std::vector<double>(X.second.size());
	for (int i=0;i<(int)ret.second.size();++i){
		ret.second[i] = a*X.second[i] + b;
	}
	return ret;
}

class PriceCaptureImpl : public PacketHandler::Impl{
public:
	PriceCaptureImpl() : PacketHandler::Impl(){}

	struct obinfo{
		Orderbook ob;
		Greeks grk;
	};

	std::map<std::string, obinfo> obmap;
	Orderbook* ob;
	Greeks* grk;

	template <class some_packet_type>
	void setLimitOrderQuotes(const some_packet_type *header){
		ob->askprices[0] = ATOI_LEN(header->ask1price) / 100.0;
		ob->askprices[1] = ATOI_LEN(header->ask2price) / 100.0;
		ob->askprices[2] = ATOI_LEN(header->ask3price) / 100.0;
		ob->askprices[3] = ATOI_LEN(header->ask4price) / 100.0;
		ob->askprices[4] = ATOI_LEN(header->ask5price) / 100.0;
		ob->bidprices[0] = ATOI_LEN(header->bid1price) / 100.0;
		ob->bidprices[1] = ATOI_LEN(header->bid2price) / 100.0;
		ob->bidprices[2] = ATOI_LEN(header->bid3price) / 100.0;
		ob->bidprices[3] = ATOI_LEN(header->bid4price) / 100.0;
		ob->bidprices[4] = ATOI_LEN(header->bid5price) / 100.0;
	}

	void setGreeks(const KrxOptionsGreek *optgreek){
		grk->delta = ATOLL_LEN_SIGN(optgreek->deltasign[0],optgreek->delta) / 1000000.0;
		grk->gamma = ATOLL_LEN_SIGN(optgreek->gammasign[0],optgreek->gamma) / 1000000.0;
		grk->theta = ATOLL_LEN_SIGN(optgreek->thetasign[0],optgreek->theta) / 1000000.0;
		grk->vega = ATOLL_LEN_SIGN(optgreek->vegasign[0],optgreek->vega) / 1000000.0;
		grk->rho = ATOLL_LEN_SIGN(optgreek->rhosign[0],optgreek->rho) / 1000000.0;
	}

	void update(long long capturedType, char *msg){
		PacketHandler::Impl::update(capturedType,msg);
		if (obmap.find(this->krcodestr)==obmap.end()){
			obmap[this->krcodestr] = obinfo();
			ob = &(obmap.find(this->krcodestr)->second.ob);
			memset(ob->askprices,0,sizeof(ob->askprices)); memset(ob->bidprices,0,sizeof(ob->bidprices));
			ob->currentprice = 0;
			ob->expectedprice = 0;
		}
		ob = &(obmap.find(this->krcodestr)->second.ob);

		switch(capturedType){
			case t_KrxFuturesTradeBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesTradeBestQuotation);
				break;
			case t_KrxOptionsTradeBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsTradeBestQuotation);
				break;
			case t_KrxFuturesTrade:
				ob->currentprice = ATOI_LEN(futheader.m_KrxFuturesTrade->currentprice) / 100.0;
				break;
			case t_KrxOptionsTrade:
				ob->currentprice = ATOI_LEN(optheader.m_KrxOptionsTrade->currentprice) / 100.0;
				break;
			case t_KrxFuturesBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesBestQuotation);
				ob->expectedprice = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->expectedprice) / 100.0;
				break;
			case t_KrxOptionsBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsBestQuotation);
				ob->expectedprice = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->expectedprice) / 100.0;
				break;
			case t_KrxOptionsGreek:
				setGreeks(optheader.m_KrxOptionsGreek);
				break;
			default:;
		}
	}
};

class BrickBase{
public:
	BrickBase(const std::vector<Brick *>& brick)
	{
		for (int i=0;i<(int)brick.size();++i)
		{
			codeVectorMap[brick[i]->pi.krcodestr].push_back(brick[i]);
		}
		
		std::map<std::string, std::vector<Brick *> >::iterator it = codeVectorMap.begin();
		for (;it != codeVectorMap.end(); ++it){
			std::sort(it->second.begin(),it->second.end(),Functional::ptr_comp<Brick>);
			codes.push_back(it->first);
		}
	}

	void report_amounts(){
		std::map<std::string, std::vector<Brick *> >::iterator it = codeVectorMap.begin();
		for (;it != codeVectorMap.end(); ++it){
			std::cout << it->first << "\t" << it->second.size() << std::endl;
		}
	}
	const std::vector<Brick *>& get(const std::string& krcode){
		if (codeVectorMap.find(krcode) == codeVectorMap.end()){
			return empty_bricks;
		}
		return codeVectorMap.find(krcode)->second;
	}

	std::vector<Brick *> empty_bricks;
	std::map<std::string, std::vector<Brick *> > codeVectorMap;
	std::vector<std::string> codes;
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

	const std::vector<Brick *>& readBlockTimeGreeks(int a_time, int b_time){
	}

	const std::vector<Brick *>& readBlockTime(int a_time, int b_time){
		std::pair<long long, long long> l_r = indexer->get_interval_within(a_time,b_time);
		DataReader* rd = psbj.rdr;
		RawDataReader& rrd = psbj.rdr->rd;
		PriceCaptureImpl* pcapimpl = new PriceCaptureImpl();
		PacketHandler pcaphd(pcapimpl);
		psbj.push(&pcaphd);
		rrd.seek(l_r.first);
		printf("-");
		while(psbj.next() && rrd.prevoffset <= l_r.second)
		{
			if (isQuotationType(rd->castedRawType)){
				bricks.push_back
					(new Brick(PacketInfo(pcapimpl->krcodestr, rrd.msg, rrd.sz, rd->castedRawType, pcapimpl->timestampi),*pcapimpl->ob));
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


#endif // readers_h__
