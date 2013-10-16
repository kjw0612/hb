#ifndef formaldata_h__
#define formaldata_h__
#include "functionals.hpp"
#include "csvparserfast.hpp"
#include "obinfo2sig.hpp"

class ObField{
public:
	ObField() : pType(NONE), arrivalTime(0), tinfo(), obook(), s1info(), obinfo() {}

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
		
		NOTYPE,
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
			obinfo->n = max(obinfo->n, val.first+1);
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

	std::vector<int> itypes(4,-1);
	for (int j=0;j<(int)names.size();++j){
		if (!strcmpitr(names[j].substr(0,4),"type")){
			if (names[j].length()==4) itypes[0] = j;
			else{
				int m = names[j][4] - '0';
				itypes[m] = j;
			}
		}
	}

	while (m = cp.getline(0)){
#ifdef _DEBUG
		if (data.size()>30000)
			break;
#endif
		vd val(names.size());
		int j=0;
		if (!strcmpitr(names[0],"type")){ // 0 col should be type col(BA/TBA/T)
		}
		for (int j=0;j<(int)names.size();++j){
			val[j] = (int)cp.lined[j];
		}
		for (int k=0;k<4;++k){
			if (itypes[k]>=0){
				int ii = itypes[k];
				char typebuf[20] = "";
				sscanf_s(cp.buf+cp.offsets[ii],"%s",typebuf, 19);
				if (k==0)
					val[ii] = ObField::toType(typebuf);
				else
					val[ii] = ObInfo::toType(typebuf);
			}
		}
		data.push_back(val);
	}
	return make_pair(names, data);
}

#include <queue>
typedef deque<int> qi;

class ObDataBase{
public:
	vs names;
	vector<ObField> obdata;

	pvi insdelsign(int floor, int cap, int ins0del1) const{
		int move = 0;
		vi is, vals;
		int multip = (ins0del1==0) ? 1 : -1;
		for (int i=0;i<(int)obdata.size();++i){
			if (obdata[i].obinfo){
				for (int j=0;j<obdata[i].obinfo->n;++j){
					if ((obdata[i].obinfo->type[j] == ObInfo::INSERTION && ins0del1==0)
						|| (obdata[i].obinfo->type[j] == ObInfo::CANCEL && ins0del1==1))
					{
						if (floor <= obdata[i].obinfo->vol[j] && obdata[i].obinfo->vol[j] <= cap)
						{
							is.push_back(i);
							if (obdata[i].obinfo->price[j] <= obdata[i].obook->bid[0].price)
								vals.push_back(1 * multip); // bid insertion
							else
								vals.push_back(-1 * multip); // ask insertion
						}
					}
				}
			}
		}
		return make_pair(is, vals);
	}

	pvi tradesign(int floor, int cap) const{
		int move = 0;
		vi is, vals;
		for (int i=0;i<(int)obdata.size();++i){
			if (floor <= obdata[i].tinfo->vol && obdata[i].tinfo->vol <= cap){
				is.push_back(i);
				vals.push_back(obdata[i].tinfo->direction);
			}
		}
		return make_pair(is, vals);
	}

	pvi pricemove() const{
		int move = 0;
		vi is, vals;
		for (int i=0;i<(int)obdata.size();++i){
			if (move = sign(obdata[i].s1info->dir_bidmove + obdata[i].s1info->dir_askmove)){
				is.push_back(i);
				vals.push_back(move);
			}
		}
		return make_pair(is, vals);
	}

	static pair<vvd, vvd> genvec(int n/*event num size*/, vvd yvec, vpvi target, int m/*queue size*/, int gap, int minn) {
		vi it(target.size(),0);
		vector<qi> targetq(target.size());
		vector<qi> nq(target.size());
		for (int j=0;j<(int)targetq.size();++j)
			for (int k=0;k<m;++k){
				targetq[j].push_front(0);
				nq[j].push_front(0);
			}

		vvd xs, ys;
		xs.reserve(n/gap);
		for (int i=minn;i<n;i+=gap){
			int c=0;
			vd row(target.size() * m);
			for (int j=0;j<(int)target.size();++j){
				while(it[j] < (int)target[j].first.size()){
					if (target[j].first[it[j]] <= i){
						targetq[j].push_back(target[j].second[it[j]]);
						nq[j].push_back(target[j].first[it[j]]);
						while ((int)targetq[j].size()>m){
							targetq[j].pop_front();
							nq[j].pop_front();
						}
						++it[j];
					}
					else{
						break;
					}
				}
				//qi::iterator ia = , ib;
				for (int k=0;k<m;++k){
					int ndelta = i - nq[j][k];
					row[c++] = exp(-ndelta / 100.) * targetq[j][k];
				}
			}
			xs.push_back(row);
			ys.push_back(yvec[i]);
		}
		return make_pair(xs,ys);
	}

	pair<vvd, vvd> tset1(int m, int gap) const{
		vvd ys(obdata.size(),vd(1,0));
		vd ychk(obdata.size(),0);
		vpvi target;
		pvi pm = pricemove(), ts = tradesign(10,10000), inss = insdelsign(10,10000,0), dels = insdelsign(10,10000,1);
		target.push_back(pm); target.push_back(ts);
		target.push_back(inss); target.push_back(dels); 
		for (int i=0;i<(int)ts.first.size();++i){
			for (int k=(i==0) ? 0 : ts.first[i-1]+1; k<=ts.first[i]; ++k){
				ys[k][0] = ts.second[i];
				ychk[k] = ts.second[i];
			}
		}
		return genvec(obdata.size(), ys, target, m, gap, ts.first[0]);
	}

	void print(const string& filepath, int complex = 0) const {
		FILE *fo = NULL;
		fopen_s(&fo,filepath.c_str(),"wt");
		if (fo){
			if (!complex){
				fprintf(fo,"arrivalTime,price,vol,direction, bidPrc1,bidQty1, askPrc1,askQty1,is_sell,is_buy,is_bidinsertion,is_askinsertion,is_bidcancelled,is_askcancelled,dir_bidmoved,dir_askmoved,");
				fprintf(fo,"\n");
				for (int i=0;i<(int)obdata.size();++i){
					if (obdata[i].s1info && obdata[i].s1info->isS1Event()){
						fprintf(fo,"%d,%d,%d,%d,",obdata[i].arrivalTime,obdata[i].tinfo->price,obdata[i].tinfo->vol,obdata[i].tinfo->direction);
						fprintf(fo,"%d,%d,%d,%d,",obdata[i].obook->bid[0].price,obdata[i].obook->bid[0].qty,obdata[i].obook->ask[0].price,obdata[i].obook->ask[0].qty);
						fprintf(fo,"%d,%d,%d,%d,",obdata[i].s1info->is_sell,obdata[i].s1info->is_buy,obdata[i].s1info->is_bidinsert,obdata[i].s1info->is_askinsert);
						fprintf(fo,"%d,%d,%d,%d,",obdata[i].s1info->is_bidcancel,obdata[i].s1info->is_askcancel,obdata[i].s1info->dir_bidmove,obdata[i].s1info->dir_askmove);
						fprintf(fo,"\n");
					}
				}
				fclose(fo);
			}
		}
	}

	void put(ObField::FieldType _ft, const vvd& _val, int _idx, int _addi = -1){
		for (int i=0;i<(int)_val.size();++i){
			int __val = (int)_val[i][_idx];
			if (_addi==-1){
				obdata[i].setValue(_ft, __val);
			}
			else{
				pi __vpi;
				__vpi.first = _addi; __vpi.second = __val;
				obdata[i].setValue(_ft, __vpi);
			}
		}
	}

	ObDataBase(const vs& _names, const vvd& _val) : names(_names)
	{
		obdata.resize(_val.size());
		for (int i=0;i<(int)names.size();++i){
			ObField::FieldType ft = ObField::NOTYPE;
			if (!strcmpitr(names[i],"TYPE") ||!strcmpitr(names[i],"ARRIVALTIME")
				|| !strcmpitr(names[i],"PRICE") || !strcmpitr(names[i],"VOL") || !strcmpitr(names[i],"DIRECTION"))
			{
				if (!strcmpitr(names[i],"TYPE")){
					ft = ObField::PACKETTYPE;
				}
				else if (!strcmpitr(names[i],"ARRIVALTIME")){
					ft = ObField::ARRIVALTIME;
				}
				else if (!strcmpitr(names[i],"PRICE")){
					ft = ObField::TRADEPRICE;
				}
				else if (!strcmpitr(names[i],"VOL")){
					ft = ObField::TRADEVOL;
				}
				else if (!strcmpitr(names[i],"DIRECTION")){
					ft = ObField::DIRECTION;
				}
				put(ft, _val, i);
			}
			else{
				int addi = 0;
				if (!strcmpitr(names[i].substr(0,6),"BIDPRC")){
					ft = ObField::BIDPRC;
					addi = names[i][6] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,6),"BIDQTY")){
					ft = ObField::BIDQTY;
					addi = names[i][6] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,6),"ASKPRC")){
					ft = ObField::ASKPRC;
					addi = names[i][6] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,6),"ASKQTY")){
					ft = ObField::ASKQTY;
					addi = names[i][6] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,5),"PRICE")){ // PRICE1, PRICE2, PRICE3
					ft = ObField::PRICE;
					addi = names[i][5] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,4),"TYPE")){ // TYPE1, TYPE2, TYPE3
					ft = ObField::TYPE;
					addi = names[i][4] - '0';
				}
				else if (!strcmpitr(names[i].substr(0,3),"VOL")){ // VOL1, VOL2, VOL3
					ft = ObField::VOL;
					addi = names[i][3] - '0';
				}
				put(ft, _val, i, addi);
			}
		}
		initPtr(obdata[0].s1info); // init by 0
		for (int i=1;i<(int)obdata.size();++i){
			if (obdata[i].s1info == NULL && obdata[i].obinfo){
				initPtr(obdata[i].s1info);
				*obdata[i].s1info = ObInfo2Sig1(*obdata[i].obinfo, obdata[i].obook->ask[0].price, obdata[i].obook->bid[0].price,
					obdata[i].tinfo->direction, obdata[i-1].obook->ask[0].price, obdata[i-1].obook->bid[0].price);
			}
		}
	}
};

#endif // formaldata_h__