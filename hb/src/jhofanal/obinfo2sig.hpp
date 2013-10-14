#ifndef obinfo2sig_h__
#define obinfo2sig_h__

#include "functionals.hpp"

class ObInfo{
public:
	enum TYPE {
		TRADE,
		INSERTION,
		CANCEL,
		NONE
	};
	ObInfo() : n(0) {}
	void add(const string& _type, int _price, int _vol){
		TYPE type_ = TRADE;
		if (!strcmpitr(_type,"trade")) type_ = TRADE;
		else if (!strcmpitr(_type,"insertion")) type_ = INSERTION;
		else if (!strcmpitr(_type,"cancel")) type_ = CANCEL;
		else if (!strcmpitr(_type,"none")) type_ = NONE;
		add(type_, _price, _vol);
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

class Sig1Info{
public:
	Sig1Info() : is_sell(), is_buy(), is_bidinsert(), is_askinsert(), is_bidcancel(), is_askcancel(), dir_bidmove(), dir_askmove(){}
	int is_sell, is_buy, is_bidinsert, is_askinsert, is_bidcancel, is_askcancel, dir_bidmove, dir_askmove;
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
	if (oask1price){
		if (oask1price < ask1price) ret.dir_askmove = 1;
		else if (oask1price > ask1price) ret.dir_askmove = -1;
	}
	else if (obid1price){
		if (obid1price < bid1price) ret.dir_bidmove = 1;
		else if (obid1price > bid1price) ret.dir_bidmove = -1;
	}
	else{
		if (direction == 1) ret.dir_askmove = 1;
		else if (direction == -1) ret.dir_bidmove = -1;
	}
	return ret;
}

#endif // obinfo2sig_h__
