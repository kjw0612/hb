#ifndef packethandler_h__
#define packethandler_h__

#include "basereaders.h"
#include "macros.h"
#include <map>
#include <string>

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
			memset(krcode,0,sizeof(krcode));
			COPY_STR(krcode, header->krcode);
			_strupr_s(krcode);
			krcodestr = krcode;
			timestampi = ATOI_LEN(header->timestamp);
		}
		virtual void update(long long capturedType, char *msg)
		{
			futheader.m_rawmsg = msg;
			optheader.m_rawmsg = msg;
			tradequantity = 0;
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
			case t_KrxOptionsGreek:
				setCodeTime(optheader.m_KrxOptionsGreek);
				break;
			default:;
			}
		}
		KrxOptionsHeader optheader;
		KrxFuturesHeader futheader;
		char krcode[20];
		std::string krcodestr;
		int timestampi;
		int tradequantity;
	};
	PacketHandler() : impl(new Impl()) {}
	PacketHandler(Impl * a_impl) : impl(a_impl) {}
	~PacketHandler(){ //delete impl;
	}
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

class PriceCaptureImpl : public PacketHandler::Impl{
public:
	PriceCaptureImpl() : PacketHandler::Impl(){}

	struct obinfo{
		Orderbook ob;
		Greeks grk;
	};

	std::map<std::string, obinfo> obmap;
	Orderbook old_ob;

	Orderbook* ob;
	Greeks* grk;

	template <class some_packet_type>
	void setLimitOrderQuotes(const some_packet_type *header){
		ob->askprices[0] = ATOI_LEN(header->ask1price);
		ob->askprices[1] = ATOI_LEN(header->ask2price);
		ob->askprices[2] = ATOI_LEN(header->ask3price);
		ob->askprices[3] = ATOI_LEN(header->ask4price);
		ob->askprices[4] = ATOI_LEN(header->ask5price);
		ob->bidprices[0] = ATOI_LEN(header->bid1price);
		ob->bidprices[1] = ATOI_LEN(header->bid2price);
		ob->bidprices[2] = ATOI_LEN(header->bid3price);
		ob->bidprices[3] = ATOI_LEN(header->bid4price);
		ob->bidprices[4] = ATOI_LEN(header->bid5price);
		/*
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
		*/
		ob->askquantities[0] = ATOI_LEN(header->ask1quantity);
		ob->askquantities[1] = ATOI_LEN(header->ask2quantity);
		ob->askquantities[2] = ATOI_LEN(header->ask3quantity);
		ob->askquantities[3] = ATOI_LEN(header->ask4quantity);
		ob->askquantities[4] = ATOI_LEN(header->ask5quantity);
		ob->bidquantities[0] = ATOI_LEN(header->bid1quantity);
		ob->bidquantities[1] = ATOI_LEN(header->bid2quantity);
		ob->bidquantities[2] = ATOI_LEN(header->bid3quantity);
		ob->bidquantities[3] = ATOI_LEN(header->bid4quantity);
		ob->bidquantities[4] = ATOI_LEN(header->bid5quantity);

		ob->asktotquantity = ATOI_LEN(header->asktotalquantity);
		ob->bidtotquantity = ATOI_LEN(header->bidtotalquantity);

		ob->asktotcnt = ATOI_LEN(header->asktotalvalidnum);
		ob->bidtotcnt = ATOI_LEN(header->bidtotalvalidnum);

		ob->askcnts[0] = ATOI_LEN(header->ask1totalvalidnum);
		ob->askcnts[1] = ATOI_LEN(header->ask2totalvalidnum);
		ob->askcnts[2] = ATOI_LEN(header->ask3totalvalidnum);
		ob->askcnts[3] = ATOI_LEN(header->ask4totalvalidnum);
		ob->askcnts[4] = ATOI_LEN(header->ask5totalvalidnum);
		ob->bidcnts[0] = ATOI_LEN(header->bid1totalvalidnum);
		ob->bidcnts[1] = ATOI_LEN(header->bid2totalvalidnum);
		ob->bidcnts[2] = ATOI_LEN(header->bid3totalvalidnum);
		ob->bidcnts[3] = ATOI_LEN(header->bid4totalvalidnum);
		ob->bidcnts[4] = ATOI_LEN(header->bid5totalvalidnum);
	}

	template <class some_packet_type>
	void setTraded(const some_packet_type *header){
		ob->currentprice = ATOI_LEN(optheader.m_KrxOptionsTrade->currentprice);
		ob->tradequantity = ATOI_LEN(header->tradequantity);
		if (((old_ob.currentprice != 0) && old_ob.currentprice > ob->currentprice) ||
			old_ob.askprices[0] > ob->askprices[0] || old_ob.bidprices[0] > ob->bidprices[0])
		{
			ob->dir = -1;
		}
		else if (((old_ob.currentprice != 0) && old_ob.currentprice < ob->currentprice) ||
			old_ob.askprices[0] < ob->askprices[0] || old_ob.bidprices[0] < ob->bidprices[0])
		{
			ob->dir = 1;
		}
		else if (old_ob.askquantities[0] > ob->askquantities[0]){
			ob->dir = -1;
		}
		else if (old_ob.bidquantities[0] > ob->bidquantities[0]){
			ob->dir = -1;
		}
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
			grk = &(obmap.find(this->krcodestr)->second.grk);
			memset(ob->askprices,0,sizeof(ob->askprices)); memset(ob->bidprices,0,sizeof(ob->bidprices));
			ob->currentprice = 0;
			ob->expectedprice = 0;
		}
		ob = &(obmap.find(this->krcodestr)->second.ob);
		grk = &(obmap.find(this->krcodestr)->second.grk);
		old_ob = *ob;
		ob->tradequantity = 0; ob->dir = 0;

		switch(capturedType){
			case t_KrxFuturesTradeBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesTradeBestQuotation);
				setTraded(futheader.m_KrxFuturesTradeBestQuotation);
				break;
			case t_KrxOptionsTradeBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsTradeBestQuotation);
				setTraded(optheader.m_KrxOptionsTradeBestQuotation);
				break;
			case t_KrxFuturesTrade:
				setTraded(futheader.m_KrxFuturesTrade);
				break;
			case t_KrxOptionsTrade:
				setTraded(optheader.m_KrxOptionsTrade);
				break;
			case t_KrxFuturesBestQuotation:
				setLimitOrderQuotes(futheader.m_KrxFuturesBestQuotation);
				ob->expectedprice = ATOI_LEN(futheader.m_KrxFuturesBestQuotation->expectedprice);
				break;
			case t_KrxOptionsBestQuotation:
				setLimitOrderQuotes(optheader.m_KrxOptionsBestQuotation);
				ob->expectedprice = ATOI_LEN(optheader.m_KrxOptionsBestQuotation->expectedprice);
				break;
			case t_KrxOptionsGreek:
				setGreeks(optheader.m_KrxOptionsGreek);
				break;
			default:;
		}
	}
};

#endif // packethandler_h__
