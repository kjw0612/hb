#ifndef formaldata_h__
#define formaldata_h__
#include "functionals.hpp"
#include "csvparserfast.hpp"
#include "obinfo2sig.hpp"

class ObField{
public:
	enum PACKET_TYPE {
		BA,TBA,T,NONE,
	};
	static PACKET_TYPE toType(const string& rhs){
		if (!strcmpitr(rhs,"BA")) return BA;
		else if (!strcmpitr(rhs,"TBA")) return TBA;
		else if (!strcmpitr(rhs,"T")) return T;
		else return NONE;
	}
	enum FieldType{
		PACKETTYPE,ARRIVALTIME,

		TRADEPRICE,TRADEVOL,DIRECTION,

		BIDPRC,BIDQTY,
		ASKPRC,ASKQTY,

		TYPE,PRICE,VOL,
	};

	PACKET_TYPE pType;
	int arrivalTime;
	shared_ptr<TradeInfo> tinfo;
	shared_ptr<OrderBook> obook;
	shared_ptr<Sig1Info> s1info;
	shared_ptr<ObInfo> obinfo;

	void setValue(FieldType ft, pi val){
		--val.first;
		switch (ft){
		case BIDPRC: case BIDQTY: 
			initPtr(obook);
			if ((int)obook->bid.size() < val.first+1){
				obook->bid.resize(val.first+1);
			}
			if (ft==BIDPRC) obook->bid[val.first].price = val.second;
			else obook->bid[val.first].qty = val.second;
			break;
		case ASKPRC: case ASKQTY:
			initPtr(obook);
			if ((int)obook->ask.size() < val.first+1){
				obook->ask.resize(val.first+1);
			}
			if (ft==ASKPRC) obook->ask[val.first].price = val.second;
			else obook->ask[val.first].qty = val.second;
			break;
		case TYPE: case PRICE: case VOL:
			initPtr(obinfo);
			if (ft==TYPE) obinfo->type[val.first] = (ObInfo::TYPE)val.second;
			else if (ft==PRICE) obinfo->price[val.first] = val.second;
			else obinfo->vol[val.first] = val.second;
			break;
		}
	}
	
	void setValue(FieldType ft, int val){
		switch (ft){
		case TRADEPRICE: case TRADEVOL: case DIRECTION: initPtr(tinfo);
			if (ft==TRADEPRICE) tinfo->price = val;
			else if (ft==TRADEVOL) tinfo->vol = val;
			else tinfo->direction = val;
			break;
		case PACKETTYPE: case ARRIVALTIME:
			if (ft==PACKETTYPE) pType = (PACKET_TYPE)val;
			else arrivalTime = val;
			break;
		}
	}
};

inline pair<vs, vvd> getDataPool(const string& filename){
	CsvParserFast cp(filename);
	int m = cp.getline(1);

	vs names;
	vvd data;

	data.reserve(200000);
	for (int i=0;i<m;++i)
		names.push_back(ospace::trim(cp.line[i]));
	while (m = cp.getline(0)){
		vd val(names.size());
		int j=0;
		if (!strcmpitr(names[0],"type")){ // 0 col should be type col(BA/TBA/T)
			char typebuf[20] = "";
			sscanf_s(cp.buf,"%s",typebuf, 19);
			val[0] = ObField::toType(typebuf);
			j = 1;
		}
		for (;j<(int)names.size();++j){
			val[j] = (int)cp.lined[j];
		}
		data.push_back(val);
	}
	return make_pair(names, data);
}


class ObDataBase{
public:
	vs names;
	vector<ObField> obdata;
	ObDataBase(const vs& _names, const vvd& _val){
		obdata.resize(_val.size());
		for (int i=0;i<(int)_names.size();++i){
		}
	}
};

#endif // formaldata_h__