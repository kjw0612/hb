#ifndef obinfo2sig_h__
#define obinfo2sig_h__

#include "functionals.hpp"

class OrderBook{
public:
	double wmprice() const{
		if (ask.size()==0 || bid.size()==0) return 0;
		return (ask[0].price*ask[0].qty + bid[0].price*bid[0].qty) / (double)(ask[0].qty+bid[0].qty);
	}
	class Container{
	public:
		int price, qty;
	};
	vector<Container> ask, bid;
};

class ObInfo{
public:
	enum TYPE{
		TRADE = 1,
		INSERTION = 2,
		CANCEL = 3,
		NONE = 0,
	};
	ObInfo() : n(0) { 
		type[0] = type[1] = type[2] = NONE;
		price[0] = price[1] = price[2] = 0;
		vol[0] = vol[1] = vol[2] = 0;
	}
	static TYPE toType(const char * rhs){
		if (toupper(rhs[0])=='C')
			return CANCEL;
		else if (toupper(rhs[0])=='I')
			return INSERTION;
		else if (toupper(rhs[0])=='T')
			return TRADE;
		else
			return NONE;
		if (!strcmpitr(rhs,"INSERTION")) return INSERTION;
		else if (!strcmpitr(rhs,"CANCEL")) return CANCEL;
		else if (!strcmpitr(rhs,"TRADE")) return TRADE;
		else return NONE;
	}
	void add(const string& _type, int _price, int _vol){
		add(toType(_type.c_str()), _price, _vol);
	}
	void add(TYPE _type, int _price, int _vol){
		type[n] = _type; price[n] = _price; vol[n] = _vol;
		++n;
	}
	int n;
	TYPE type[3];
	int price[3];
	int vol[3];
};

class TradeInfo{
public:
	int price, vol, direction;
};

class Sig1Info{
public:
	Sig1Info() : is_sell(), is_buy(), is_bidinsert(), is_askinsert(), is_bidcancel(), is_askcancel(), dir_bidmove(), dir_askmove(){}
	int is_sell, is_buy, is_bidinsert, is_askinsert, is_bidcancel, is_askcancel, dir_bidmove, dir_askmove;
	int isS1Event() const{
		return is_sell || is_buy || is_bidinsert || is_askinsert || is_bidcancel || is_askcancel || dir_bidmove || dir_askmove;
	}
};

inline Sig1Info ObInfo2Sig1(const ObInfo& obinfo, int ask1price, int bid1price, int direction, int oask1price, int obid1price){
	Sig1Info ret;
	for (int i=0;i<obinfo.n;++i){
		if (obinfo.type[i] == ObInfo::TRADE){
			if (obinfo.price[i]==ask1price) ret.is_buy = 1;
			if (obinfo.price[i]==bid1price) ret.is_sell = 1;
		}
		if (obinfo.type[i] == ObInfo::INSERTION){
			if (obinfo.price[i]==ask1price) ret.is_askinsert = 1;
			if (obinfo.price[i]==bid1price) ret.is_bidinsert = 1;
		}
		if (obinfo.type[i] == ObInfo::CANCEL){
			if (obinfo.price[i]==ask1price) ret.is_askcancel = 1;
			if (obinfo.price[i]==bid1price) ret.is_bidcancel = 1;
		}
	}
	if ((oask1price && ask1price) || (obid1price && bid1price)){
		if (oask1price && ask1price){
			if (oask1price < ask1price) ret.dir_askmove = 1;
			else if (oask1price > ask1price) ret.dir_askmove = -1;
		}
		if (obid1price && bid1price){
			if (obid1price < bid1price) ret.dir_bidmove = 1;
			else if (obid1price > bid1price) ret.dir_bidmove = -1;
		}
	}
	else{
		if (direction == 1){
			ret.is_buy = 1;
			ret.dir_askmove = 1;
		}
		else if (direction == -1){
			ret.is_sell = 1;
			ret.dir_bidmove = -1;
		}
	}
	return ret;
}

#endif // obinfo2sig_h__
