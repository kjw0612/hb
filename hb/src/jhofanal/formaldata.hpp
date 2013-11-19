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

	static PACKET_TYPE toType(const char * rhs){
		if (toupper(rhs[0]) == 'B')
			return BA;
		if (toupper(rhs[0]) == 'T'){
			if (toupper(rhs[1]) == 'B')
				return TBA;
			return T;
		}
		return NONE;
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
	double arrivalTime;
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
	
	void setValue(FieldType ft, double val){
		switch (ft){
		case TRADEPRICE: case TRADEVOL: case DIRECTION: initPtr(tinfo);
			if (ft==TRADEPRICE) tinfo->price = (int)val;
			else if (ft==TRADEVOL) tinfo->vol = (int)val;
			else tinfo->direction = (int)val;
			break;
		case PACKETTYPE: case ARRIVALTIME:
			if (ft==PACKETTYPE) pType = (PACKET_TYPE)(int)val;
			else arrivalTime = val;
			break;
		}
	}
};

inline pair<vs, vvd> getDataPool(const string& filename){
	printf("-");
#ifdef _USE_BOOST_
	boost::timer timer_;
#endif
	CsvParserFast cp(filename);
	int m = cp.getrow	(1);

	vs names;
	vvd data;


	data.reserve(300000);
	for (int i=0;i<m;++i)
		names.push_back(ospace::trim(cp.line[i]));

	int mm = names.size();

	std::vector<int> itypes(4,-1);
	for (int j=0;j<mm;++j){
		if (!strcmpitr(names[j].substr(0,4),"type")){
			if (names[j].length()==4) itypes[0] = j;
			else{
				int m = names[j][4] - '0';
				itypes[m] = j;
			}
		}
	}

	while (m = cp.getrow(0)){
#ifdef _DEBUG
		if (data.size()>100000)
			break;
#endif
		//continue;
		vd val(mm);
		int j=0;
		for (int j=0;j<mm;++j){
			val[j] = cp.lined[j];
		}
		for (int k=0;k<4;++k){
			if (itypes[k]>=0){
				int ii = itypes[k];
				char typebuf[20] = "";
				if (k==0)
					val[ii] = ObField::toType(cp.buf+cp.offsets[ii]);
				else
					val[ii] = ObInfo::toType(cp.buf+cp.offsets[ii]);
				/*
				sscanf_s(cp.buf+cp.offsets[ii],"%s",typebuf, 19);
				if (k==0)
					val[ii] = ObField::toType(typebuf);
				else
					val[ii] = ObInfo::toType(typebuf);
					*/
			}
		}
		data.push_back(val);
	}
#ifdef _USE_BOOST_
	std::cout << timer_.elapsed() << std::endl;
#endif
	return make_pair(names, data);
}

#include <queue>
typedef deque<int> qi;

class ObDataBase{
public:
	vs names;
	vector<ObField> obdata;
	mutable vector<ObField> obdata_only_trades;
	mutable vi obdatais;

	unsigned int size() const{
		return obdata.size();
	}

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

	
	static vd val_to_diff(int ndelta, const vd& accs){
		vd ret = accs;
		int n = accs.size(), sti = 0;
		for (int i=0;i<n;++i){
			int ns = (i<ndelta) ? i+1 : ndelta;
			int st = (i<ndelta) ? i-1 : i-ndelta;
			double diff = (st<0) ? accs[i] : accs[i] - accs[st];
			ret[i] = diff / ns;
		}
		return ret;
	}

	static void sidecut(vd& rhs, int ndelta){
		int n = rhs.size(), sti = 0;
		for (int i=0;i<n;++i){
			if (sti <= 0 && rhs[i]){
				sti = i;
				break;
			}
		}
		rhs[n-1] = rhs[n-2];
		for (int i=sti+ndelta;i>=sti;--i){
			rhs[i] = rhs[i+1];
		}

	}

	mutable pair<vi, vd> pnls;
	set<int> selis;
	mutable set<int> wholeis;

	void idx_set(const set<int>& idxset){
		selis = idxset;
	}

	set<int> idx_volminmax(int minVol, int maxVol) const{
		set<int> ret; 
		for (int i=0;i<(int)obdata.size();++i)
			if(obdata[i].tinfo->vol >= minVol && obdata[i].tinfo->vol <= maxVol) ret.insert(i);
		return ret;
	}
	set<int> idx_conseqadj(int interval_milliseconds) const{
		set<int> ret; 
		for (int i=0;i<(int)obdata.size();++i)
			if ((i>0 && obdata[i].arrivalTime - obdata[i-1].arrivalTime < (interval_milliseconds/10.))
				|| (i<(int)obdata.size()-1 && obdata[i+1].arrivalTime - obdata[i].arrivalTime < (interval_milliseconds/10.)))
				ret.insert(i);
		return ret;
	}
	set<int> idx_pricemoveadj(int before_milliseconds, int after_milliseconds) const{
		set<int> ret; 
		int j=0;
		for (int i=0;i<(int)obdata.size();++i)
			if (obdata[i].s1info->dir_bidmove || obdata[i].s1info->dir_askmove){
				while (obdata[i].arrivalTime - before_milliseconds/10. < obdata[j].arrivalTime){ ++j; }
				while (obdata[j].arrivalTime <= obdata[i].arrivalTime + after_milliseconds/10.){ ret.insert(j++); }
			}
		return ret;
	}

	void clear_idx(){
		selis.clear();
	}

	void setiset(set<int>& iset) const {
		if (iset.size()==0){
			if (selis.size()==0){
				if (wholeis.size()==0){
					for (int i=0;i<(int)obdata.size();++i)
						wholeis.insert(i);
				}
				iset = wholeis;
			}
			else
				iset = selis;
		}
	}


	pair<vi, vi> pmseq_strict() const {
		pair<vi, vi> ret;
		int amove=0,bmove=0;
		for (int i=1;i<(int)obdata.size();++i){
			if (obdata[i].obook->ask[0].price > obdata[i-1].obook->ask[0].price)
				amove = 1;
			else if (obdata[i].obook->ask[0].price < obdata[i-1].obook->ask[0].price)
				amove = -1;
			if (obdata[i].obook->bid[0].price > obdata[i-1].obook->bid[0].price)
				bmove = 1;
			else if (obdata[i].obook->bid[0].price < obdata[i-1].obook->bid[0].price)
				bmove = -1;
			if (amove == bmove && amove!=0){
				ret.first.push_back(i);
				ret.second.push_back(amove);
				amove = bmove = 0;
			}
		}
		return ret;
	}

	pair<vi, vd> wmprices() const{
		vi is; vd prcs;
		for (int i=0;i<(int)obdata.size();++i){
			is.push_back(i);
			prcs.push_back(obdata[i].obook->wmprice());
			if (prcs[i]==0 && i>0)
				prcs[i] = prcs[i-1];
		}
		return make_pair(is, prcs);
	}

	pair<vi, vd> wmpdeltas(int ndelta) const{
		pair<vi, vd> ret = wmprices();
		ret.second = val_to_diff(ndelta, ret.second);
		sidecut(ret.second, ndelta);
		return ret;
	}

	pvi tradeqty() const{
		vi is, qtys;
		for (int i=0;i<(int)obdata.size();++i){
			if (obdata[i].tinfo->vol){
				is.push_back(i);
				qtys.push_back(obdata[i].tinfo->vol * obdata[i].tinfo->direction);
			}
		}
		return make_pair(is, qtys);
	}

	pair<vi, vd> getpnls(set<int> iset = set<int>()) const{ // (caveat)! pnl domain is everywhere.)
		setiset(iset);
		double position = 0, avgprice = 0, pnl = 0;
		double realizedpnl = 0, potentialpnl = 0;
		pair<vi, vd> wmp = wmprices();

		int j=0;
		vi is; vd retpnl;
		is.resize(obdata.size()); retpnl.resize(obdata.size());
		for (int i=0;i<(int)obdata.size();++i){
			if (obdata[i].tinfo->vol >= 1){
				if (iset.find(i) != iset.end()){
					double dpos = obdata[i].tinfo->vol * obdata[i].tinfo->direction;
					double davgprice = obdata[i].tinfo->price;
					if (position + dpos == 0) avgprice = 0;
					else avgprice = (position * avgprice + dpos * davgprice) / (position + dpos);
					position += dpos;
					realizedpnl += (-dpos * davgprice);
				}
			}
			is[i] = i;
			potentialpnl = position * wmp.second[i];
			retpnl[i] = realizedpnl + potentialpnl;
		}
		return make_pair(is, retpnl);
	}

	pair<vi, vd> acclimit(int isbid, int isask, set<int> iset = set<int>()) const{
		setiset(iset);
		vi is; double acc = 0, dacc; vd accs;
		for (set<int>::const_iterator it=iset.begin();it!=iset.end();++it){
			int i = (*it);
			if (obdata[i].obinfo != NULL){
				dacc = 0;
				for (int j=0;j<3;++j){
					int multip = 0;
					if (obdata[i].obinfo->type[j] == ObInfo::INSERTION) multip = 1;
					if (obdata[i].obinfo->type[j] == ObInfo::CANCEL) multip = -1;
					if (obdata[i].obook->bid[0].price >= obdata[i].obinfo->price[j]){ // bid event
						if (isbid) dacc += obdata[i].obinfo->vol[j] * multip;
					}
					else if (isask) // ask event
						dacc += obdata[i].obinfo->vol[j] * multip;
				}
				if (dacc){
					acc += dacc;
					is.push_back(i);
					accs.push_back(acc);
				}
			}
		}
		return make_pair(is, accs);
	}

	pair<vi, vd> obookslope(set<int> iset = set<int>()) const{
		setiset(iset);
		vi is; vd slopes; double slope = 0.5;
		for (set<int>::const_iterator it=iset.begin();it!=iset.end();++it){
			int i = (*it);
			if (obdata[i].obook == NULL);
			else if (obdata[i].obook->ask.size()>0 && (obdata[i].obook->ask[0].qty+obdata[i].obook->bid[0].qty)>0)
				slope = obdata[i].obook->bid[0].qty / (double)(obdata[i].obook->ask[0].qty+obdata[i].obook->bid[0].qty);
			is.push_back(i);
			slopes.push_back(slope);
		}
		return make_pair(is,slopes);
	}

	pair<vi, vd> accumqty(set<int> iset = set<int>(), int isabs = 0) const{
		setiset(iset);
		vi is; vd accumqtys; double accumqty = 0;
		for (set<int>::const_iterator it=iset.begin();it!=iset.end();++it){
			int i = (*it);
			if (obdata[i].tinfo->vol >= 1){
				is.push_back(i);
				if (!isabs) accumqty += obdata[i].tinfo->vol * obdata[i].tinfo->direction;
				else accumqty += obdata[i].tinfo->vol;
				accumqtys.push_back(accumqty);
			}
		}
		pnls = getpnls(iset);
		return make_pair(is, accumqtys);
	}

	pair<vi, vd> accumqtyabs(set<int> iset = set<int>()) const{
		return accumqty(iset, 1);
	}

	pair<vi, vd> accumqtyabsavg(int ndelta, set<int> iset = set<int>()) const{
		pair<vi, vd> ret = accumqtyabs(iset);
		ret.second = val_to_diff(ndelta, ret.second);
		sidecut(ret.second, ndelta);
		return ret;
	}

	pvi tradesign(set<int> iset = set<int>()) const{
		setiset(iset);
		int move = 0;
		vi is, vals;
		for (set<int>::const_iterator it=iset.begin();it!=iset.end();++it){
			int i = (*it);
			is.push_back(i);
			vals.push_back(obdata[i].tinfo->direction);
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

	static pair<vi, vd> tdensity(const vi& rhs, int nbasket){
		vi is; vd dens;
		for (int i=0;i<(int)rhs.size();++i){
			int count = 0;
			for (int j=i;j>=0;--j){
				if (rhs[i] - rhs[j] <= nbasket)
					++count;
				else break;
			}
			is.push_back(rhs[i]);
			dens.push_back((int)count);
		}
		return make_pair(is, dens);
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

	enum TSET_TYPE{
		PRICE_MOVE = 0x0001,
		TRADE_SIGN = 0x0010,
		INS_SIGN = 0x0100,
		DEL_SIGN = 0x1000,
		TSET1 = 0x1111,
		TSET2 = 0x0001,
	};

	pair<vvd, vvd> tset1(int m, int gap, TSET_TYPE type) const{
		vvd ys(obdata.size(),vd(1,0));
		vd ychk(obdata.size(),0);
		vpvi target;
		pvi ts = tradesign(idx_volminmax(10,10000));
		pvi pm = pricemove();

		if (type | PRICE_MOVE){
			target.push_back(pm);
		}
		if (type | TRADE_SIGN){
			target.push_back(ts);
		}
		if (type | INS_SIGN){
			pvi inss = insdelsign(10,10000,0);
			target.push_back(inss);
		}
		if (type | DEL_SIGN){
			pvi dels = insdelsign(10,10000,1);
			target.push_back(dels); 
		}
		for (int i=0;i<(int)pm.first.size();++i){
			for (int k=(i==0) ? 0 : pm.first[i-1]+1; k<=pm.first[i]; ++k){
				ys[k][0] = pm.second[i];
				ychk[k] = pm.second[i];
			}
		}
		return genvec(obdata.size(), ys, target, m, gap, ts.first[0]);
	}

	pair<vs, int> tset1vsnn(int m, int gap, TSET_TYPE type) const{
		vs xnames;
		for (int i=0;i<m;++i){
			if (type | PRICE_MOVE)
				xnames.push_back("pricemove");
			if (type | TRADE_SIGN)
				xnames.push_back("tradesign");
			if (type | INS_SIGN)
				xnames.push_back("inssign");
			if (type | DEL_SIGN)
				xnames.push_back("delsign");
		}
		return make_pair(xnames, 1);
		//return make_pair(xnames, m);
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
			double __val = _val[i][_idx];
			if (_addi==-1){
				obdata[i].setValue(_ft, __val);
			}
			else{
				pi __vpi;
				__vpi.first = _addi; __vpi.second = (int)__val;
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