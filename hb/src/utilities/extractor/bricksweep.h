#ifndef bricksweep_h__
#define bricksweep_h__

#include "readerstatic.h"

class BrickSweep{
public:
	char type; // 'f', 'c', 'p'
	std::string krcode;
	std::string expirydate;
	int bricksize;
	int quantity;
	double delta;
	double bidticksize;
	double askticksize;
	double bidaskspread;
	double basp_futunit;
	double bidfutunit;
	double askfutunit;

	Brick::GetDataType getdatatype;

	mutable double diff_total;
	mutable double prev_price;
	mutable double current_price;

	BrickSweep(Brick::GetDataType getdatatype = Brick::MidPrice)
		: krcode(""), getdatatype(getdatatype), prev_price(-1), current_price(-1), diff_total(0) {}

	static BrickSweep getInfo(const std::string _krcode){
		BrickSweep ret;
		const std::vector<Brick *>& cb = ReaderStatic::get().callbase().get(_krcode);
		const std::vector<Brick *>& pb = ReaderStatic::get().putbase().get(_krcode);
		const std::vector<Brick *>& fb = ReaderStatic::get().futbase().get(_krcode);
		const std::vector<Brick *>& gb = ReaderStatic::get().greeksbase().get(_krcode);
		const std::vector<Brick *>* brick;
		if (cb.size()>0 && gb.size()>0){
			brick = &cb;
			ret.delta = gb.back()->grk.delta;
			ret.type = 'c';
		}
		else if (pb.size()>0 && gb.size()>0){
			brick = &pb;
			ret.delta = gb.back()->grk.delta;
			ret.type = 'p';
		}
		else if (fb.size()>0){
			brick = &fb;
			ret.delta = 1;
			ret.type = 'f';
		}
		else{
			return ret;
		}
		ret.krcode = _krcode;
		ret.expirydate = ReaderStatic::get().ds().descmap[_krcode].expirydate;
		ret.bricksize = brick->size();
		ret.quantity = (int)(brick->front()->ob.askquantities[0] + brick->front()->ob.bidquantities[0]);
		ret.bidticksize = brick->front()->ob.bidprices[0] - brick->front()->ob.bidprices[1];
		ret.askticksize = brick->front()->ob.askprices[1] - brick->front()->ob.askprices[0];
		ret.bidaskspread = brick->front()->ob.askprices[0] - brick->front()->ob.bidprices[0];
		ret.basp_futunit = ret.bidaskspread / ret.delta;
		ret.bidfutunit = ret.bidticksize / ret.delta;
		ret.askfutunit = ret.askticksize / ret.delta;
		return ret;
	}

	void setInfo(Brick * brick){
		switch(brick->type){
			case Brick::OrderBook:
				this->prev_price = this->current_price;
				this->current_price = brick->getValue(getdatatype);
				if (this->prev_price != -1)
					this->diff_total += (this->current_price - this->prev_price);
				break;
			case Brick::Greek:
				this->delta = brick->grk.delta;
				break;
			case Brick::OrderBookGreeks:
				this->prev_price = this->current_price;
				this->current_price = brick->getValue(getdatatype);
				if (this->prev_price != -1)
					this->diff_total += (this->current_price - this->prev_price);
				this->delta = brick->grk.delta;
				break;
		}
	}

	void resetDiff() const {
		this->prev_price = this->current_price;
		this->diff_total = 0;
	}

	bool empty() const {
		return krcode.length()==0;
	}
};

class BSBase{
public:
	void setInfo(const std::string& krcode, Brick * brick){
		if (relmap.find(krcode) == relmap.end()){
			relmap[krcode] = BrickSweep::getInfo(krcode);
		}
		relmap[krcode].setInfo(brick);
	}
	std::pair<double, int> diffNormPair(){
		std::vector<std::pair<int, double> > weight_diff;
		std::map<std::string, BrickSweep>::const_iterator it = relmap.begin();
		int total_activityrate = 0;
		for (;it!=relmap.end();++it){
			double fut_expected_diff = (it->second.diff_total) / it->second.delta;
			int activityrate = it->second.bricksize;
			weight_diff.push_back(std::make_pair(activityrate, fut_expected_diff));
			total_activityrate += activityrate;
		}
		double tot_weighted_diff = 0;
		for (int i=0;i<(int)weight_diff.size();++i){
			tot_weighted_diff += weight_diff[i].first * weight_diff[i].second;
		}
		if (total_activityrate==0){
			return std::make_pair(0,0);
		}
		double avg_weighted_diff = tot_weighted_diff / total_activityrate;
		return std::make_pair(avg_weighted_diff, total_activityrate);
	}
	void resetDiffAll(){
		std::map<std::string, BrickSweep>::const_iterator it = relmap.begin();
		for (;it!=relmap.end();++it){
			it->second.resetDiff();
		}
	}
	std::map<std::string, BrickSweep> relmap;
};

#endif // bricksweep_h__
