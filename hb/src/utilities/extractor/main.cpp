#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <math.h>
#include "brickbase.h"
#include "readerset.h"
#include "indexer.h"
#include "plotter.h"
#include "readerstatic.h"
#include "analysis_trajectory.h"
#include "bricksweep.h"
#include "statistics.h"

#ifdef BOOST
#include <boost/timer.hpp>
#endif

typedef std::vector<int> v_i;
typedef std::vector<double> v_d;

void setup(){
	ReaderStatic::get().setupPath_1();
	ReaderStatic::get().setupDesc_2();
	DescSet & ds_to_watch = ReaderStatic::get().ds();
	ds_to_watch.report_to_file("desc.txt");
	ReaderStatic::get().setupIndex_3();
}

void setup_time(int start_time, int end_time,
				int dmask = (ReaderStatic::FUT | ReaderStatic::CALL | ReaderStatic::PUT | ReaderStatic::GREEK))
{
	ReaderStatic::get().setupDataTime_4(start_time, end_time, dmask);
}

void plot_trajectory(int start_time, int end_time, int scenario){
	const std::vector<Brick *>& futbrick = ReaderStatic::get().futbase().get("KR4101H60001");
	std::pair<v_i, v_d> futSeries = bricks2MidPriceGrid(futbrick, start_time, end_time, 5000, Brick::Ask1);
	std::pair<v_i, v_d> futSeriesWeighted = bricks2MidPriceGrid(futbrick, start_time, end_time, 5000, Brick::WeightedMidPrice);
	double fut_price0 = futSeries.second[0];
	std::pair<v_i, v_d> c2550_futTrajectory = get_trajectory(fut_price0, "KR4201H52550", 'c', start_time, end_time, Brick::Ask1);
	std::pair<v_i, v_d> p2550_futTrajectory = get_trajectory(fut_price0, "KR4301H52558", 'p', start_time, end_time, Brick::Bid1);
	std::pair<v_i, v_d> cMinusPTrajectory2550 =
		get_synthetic_trajectory(fut_price0, "KR4201H52550", 'c', "KR4301H52558", 'p', start_time, end_time, Brick::Ask1, Brick::Bid1);
	std::pair<v_i, v_d> cMinusPTrajectory2525 =
		get_synthetic_trajectory(fut_price0, "KR4201H52527", 'c', "KR4301H52525", 'p', start_time, end_time, Brick::Ask1, Brick::Bid1);
	std::pair<v_i, v_d> cMinusPTrajectory2575 =
		get_synthetic_trajectory(fut_price0, "KR4201H52576", 'c', "KR4301H52574", 'p', start_time, end_time, Brick::Ask1, Brick::Bid1);


	Plotter plotter("A research for option Indicator");

	plotter.addPlot(futSeries,"Futures Trajectory");
	if (scenario==1){
		plotter.addPlot(c2550_futTrajectory,"Call 255.0 Delta Adjusted Trajectory");
		plotter.addPlot(p2550_futTrajectory,"Put 255.0 Delta Adjusted Trajectory");
		plotter.addPlot(cMinusPTrajectory2550,"Call - Put 255.0 Delta Adjusted Trajectory");
	}
	else{
		plotter.addPlot(cMinusPTrajectory2550,"Call - Put 255.0 Delta Adjusted Trajectory");
		plotter.addPlot(cMinusPTrajectory2525,"Call - Put 252.5 Delta Adjusted Trajectory");
		plotter.addPlot(cMinusPTrajectory2575,"Call - Put 257.5 Delta Adjusted Trajectory");
	}
	//plotter.addPlot(c2550_futTrajectory,"Call 255.0 Delta Adjusted Trajectory");
	//plotter.addPlot(p2550_futTrajectory,"Put 255.0 Delta Adjusted Trajectory");
	//plotter.addPlot(futSeries,"Futures Trajectory");
	//plotter.addPlot(futSeriesWeighted,"Futures Trajectory Weighted");
	//plotter.addPlot(cMinusPTrajectory2550,"Call - Put 255.0 Delta Adjusted Trajectory");
	//plotter.addPlot(cMinusPTrajectory2525,"Call - Put 252.5 Delta Adjusted Trajectory");
	//plotter.addPlot(cMinusPTrajectory2575,"Call - Put 257.5 Delta Adjusted Trajectory");
	plotter.refreshForever();
}

void run(int start_time, int end_time){
	Plotter plotter("A research for synthetic Indicator");
	const std::vector<Brick *>& cgreek2550 = ReaderStatic::get().greeksbase().get("KR4201H52550");
	const std::vector<Brick *>& pgreek2550 = ReaderStatic::get().greeksbase().get("KR4301H52558");
	const std::vector<Brick *>& futbrick = ReaderStatic::get().futbase().get("KR4101H60001");
	const std::vector<Brick *>& cbrick2550 = ReaderStatic::get().callbase().get("KR4201H52550");
	const std::vector<Brick *>& pbrick2550 = ReaderStatic::get().putbase().get("KR4301H52558");

	std::pair<v_i, v_d> cg2550series = bricks2MidPriceGrid(cgreek2550, start_time, end_time, 5000, Brick::Delta);
	std::pair<v_i, v_d> pg2550series = bricks2MidPriceGrid(pgreek2550, start_time, end_time, 5000, Brick::Delta);
	std::pair<v_i, v_d> c2550series = bricks2MidPriceGrid(cbrick2550, start_time, end_time, 5000);
	std::pair<v_i, v_d> p2550series = bricks2MidPriceGrid(pbrick2550, start_time, end_time, 5000);
	std::pair<v_i, v_d> cMinusPseries = aXpbY(1, c2550series, -1, p2550series);
	std::pair<v_i, v_d> futSeries = bricks2MidPriceGrid(futbrick, start_time, end_time, 5000);
	std::pair<v_i, v_d> futShift = aXpb(1, futSeries ,-250);
	std::pair<v_i, v_d> futMcall = aXpbY(1, futShift, -1, cMinusPseries);

	plotter.addPlot(c2550series,"Call 255.0");
	plotter.addPlot(p2550series,"Put 255.0");
	plotter.addPlot(cMinusPseries,"Call 255.0 - Put 255.0");
	plotter.addPlot(futShift,"Future - 250");
	plotter.addPlot(futMcall,"Future - Synthetic Future");
	plotter.refreshForever();
	return;
	//for (int i=0;i<(int))
	/*
	FILE *fo = fopen("output.csv","wt");
	for (int i=0;i<(int)c2550series.first.size();++i){
		fprintf(fo,"%d,%lf\n",c2550series.first[i], c2550series.second[i]);
	}
	fclose(fo);*/
}

std::vector<std::pair<std::string, double> > first_prices_base(const BrickBase& bbase)
{
	std::vector<std::pair<std::string, double> > ret;
	std::map<std::string, std::vector<Brick *> >::const_iterator it = bbase.codeVectorMap.begin();
	for (;it!=bbase.codeVectorMap.end();++it){
		const std::vector<Brick *>& brk = it->second;
		ret.push_back(std::make_pair(it->first, brk.back()->midPriceSimpleAvg()));
	}
	return ret;
}

void print_firstprices_base(FILE *fo, const BrickBase& bbase)
{
	std::vector<std::pair<std::string, double> > fpb = first_prices_base(bbase);
	const DescSet& dsc = ReaderStatic::get().ds();
	for (int i=0;i<(int)fpb.size();++i){
		std::string krcode = fpb[i].first;
		double strike = dsc.descmap.find(krcode)->second.strike;
		fprintf(fo,"%s,%lf,%lf\n",krcode.c_str(),fpb[i].second/100,strike);
	}
}

void prereport(){
	// this reports each option/futures' minimal unit change of corresponding futures' price.
	// code, expirydate, # trades, amount, delta, bidticksize, askticksize, bidfutsize, askfutsize
	FILE *fo;
	fopen_s(&fo,"unit_impact_on_futprice.csv","wt");
	const DescSet& dsc = ReaderStatic::get().ds();
	std::map<std::string, DescSet::Desc>::const_iterator it = dsc.descmap.begin();
	fprintf(fo,"code, expirydate, # trades, amount, delta, bidticksize, askticksize, bidfutunit, askfutunit, baspread, badelta\n");
	for (;it!=dsc.descmap.end();++it){
		std::string str = it->first;
		BrickSweep ri = BrickSweep::getInfo(str);
		// code, expirydate, # trades, amount, delta, bidticksize, askticksize
		if (!ri.empty()){
			fprintf(fo,"%s, %s, %d, %d, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n",
				ri.krcode.c_str(), ri.expirydate.c_str(), 
				ri.bricksize, ri.quantity, ri.delta, ri.bidticksize, ri.askticksize,
				ri.bidfutunit, ri.askfutunit, ri.bidaskspread, ri.basp_futunit);
		}
	}
	print_firstprices_base(fo,ReaderStatic::get().futbase());
	print_firstprices_base(fo,ReaderStatic::get().callbase());
	print_firstprices_base(fo,ReaderStatic::get().putbase());
}

void momentum_report(){
	printf("report start\n");
	std::vector<Brick *> allbrick;
	{
		std::vector<Brick *> cb = ReaderStatic::get().callbase().getAll();
		VECTOR_APPEND(cb,allbrick);
	}
	{
		std::vector<Brick *> pb = ReaderStatic::get().putbase().getAll();
		VECTOR_APPEND(pb,allbrick);
	}
	{
		std::vector<Brick *> fb = ReaderStatic::get().futbase().getAll();
		VECTOR_APPEND(fb,allbrick);
	}
	{
		std::vector<Brick *> gb = ReaderStatic::get().futbase().getAll();
		VECTOR_APPEND(gb,allbrick);
	}
	std::sort(allbrick.begin(),allbrick.end(), Functional::ptr_comp<Brick>);
	std::string targetFuture = "KR4101H60001";
	std::string targetCall = "KR4201H5";
	std::string targetPut = "KR4301H5";
	double fut_prev_prev = -1;
	double fut_prev = -1;
	int nHold = 0, nUp = 0, nUpHit = 0, nUpNotHit = 0, nUpNoSignal = 0;
	int nDown = 0, nDownHit = 0, nDownNotHit = 0, nDownNoSignal = 0;
	int nPHold = 0, nPUp = 0, nPUpHit = 0, nPUpNotHit = 0, nPUpNoSignal = 0;
	int nPDown = 0, nPDownHit = 0, nPDownNotHit = 0, nPDownNoSignal = 0;
	int activityrate = 0;
	double potential = 0;
	BSBase bsweepbase;
	std::vector<int> tstamps;
	std::vector<char> types;
	double diff_threshold = 0.01;
	int previous_time = -1;
	int current_time = -1;
	int region_sttime, region_endtime;
	std::vector<std::pair<int, int> > timescales;

	//double diff_region = 0.25;
	double diff_region = 0;
	int nDiffRegion=0;
	double tot_timescales = 0; //¶ßµç..
	printf("Region : %lf%%\n", diff_region * 100);
	std::vector<bool> flags;

	for (int i=0;i<(int)allbrick.size();++i){
		if (!STRCMPI_FRONT(allbrick[i]->krcode,targetFuture)){
			double current_price = allbrick[i]->midPriceSimpleAvg();
			if (fut_prev == current_price || allbrick[i]->hollowMidPrice()){
				continue;
			}
			bool flag = 0;
			current_time = allbrick[i]->pi.timestamp;
			if (previous_time != -1){
				int previous_seq = Functional::timestamp2seq(previous_time);
				int current_seq = Functional::timestamp2seq(current_time);
				int region_stseq = (int)((double)previous_seq * (1-diff_region) + (double)current_seq * diff_region);
				int region_endseq = (int)((double)previous_seq * diff_region + (double)current_seq * (1-diff_region));
				tot_timescales += region_endseq - region_stseq;
				++nDiffRegion;
				region_sttime = Functional::seq2timestamp(region_stseq);
				region_endtime = Functional::seq2timestamp(region_endseq);
				if (region_endseq - region_stseq > 0){
					flag = 1;
				}
			}
			else{
				region_sttime = 0;
				region_endtime = current_time;
			}
			flag = 1;
			flags.push_back(flag);
			timescales.push_back(std::make_pair(region_sttime, region_endtime));
			previous_time = current_time;
			fut_prev = current_price;
		}
	}
	timescales.push_back(std::make_pair(current_time, 1000000000));

	int regc = 0;

	for (int i=0;i<(int)allbrick.size();++i){
		/*
		std::string krcode = allbrick[i]->krcode;
		int futFlag = STRCMPI_FRONT(allbrick[i]->krcode,targetFuture);
		int callFlag = STRCMPI_FRONT(allbrick[i]->krcode,targetCall);
		int putFlag = STRCMPI_FRONT(allbrick[i]->krcode,targetPut);
		*/

		if (!STRCMPI_FRONT(allbrick[i]->krcode,targetFuture)){
			double current_price = allbrick[i]->midPriceSimpleAvg();
			//(!allbrick[i]->hollowMidPrice())
			if (fut_prev == current_price || allbrick[i]->hollowMidPrice()){
				++nHold;
			}
			else{
				std::pair<double, int> npair = bsweepbase.diffNormPair();
				activityrate = npair.second;
				potential = npair.first;
				if (fut_prev == -1);
				else{
					if (current_price > fut_prev){ // option explains future
						++nUp;
						if (fabs(potential) < diff_threshold)
							++nUpNoSignal;
						else if (potential > 0)
							++nUpHit;
						else
							++nUpNotHit;
					}
					else{
						++nDown;
						if (fabs(potential) < diff_threshold)
							++nDownNoSignal;
						else if (potential < 0)
							++nDownHit;
						else
							++nDownNotHit;
					}
					if (fut_prev_prev == -1); // future explains option
					else{
						if (fut_prev > fut_prev_prev){
							++nPUp;
							if (fabs(potential) < diff_threshold / 2 || !flags[regc])
								++nPUpNoSignal;
							else if (potential > 0)
								++nPUpHit;
							else
								++nPUpNotHit;
						}
						else{
							++nPDown;
							if (fabs(potential) < diff_threshold / 2 || !flags[regc])
								++nPDownNoSignal;
							else if (potential < 0)
								++nPDownHit;
							else
								++nPDownNotHit;
						}
					}
					fut_prev_prev = fut_prev;
				}
				++regc;
				bsweepbase.resetDiffAll();
				fut_prev = current_price;
			}
		}
		else if (!STRCMPI_FRONT(allbrick[i]->krcode,targetCall)
			|| !STRCMPI_FRONT(allbrick[i]->krcode,targetPut))
		{
			if (timescales[regc].first <= allbrick[i]->pi.timestamp
				&& allbrick[i]->pi.timestamp <= timescales[regc].second)
			{
				bsweepbase.setInfo(allbrick[i]->krcode, allbrick[i]);
			}
			else{
				int sung = 500;
			}
		}
		//switch(allbrick[i]->type){
		//}
	}
	printf("Average Region : %lf\n",tot_timescales/nDiffRegion);
	printf("\nHold: %d\n",nHold);
	printf("[how option explains future]\n");
	printf("(%lf ÀûÁß·ü)Up: %d Up Hit: %d Up Not Hit : %d Up No Signal : %d\n",
		(double)nUpHit/(nUpNotHit+nUpHit)*100,nUp,nUpHit,nUpNotHit,nUpNoSignal);
	printf("(%lf ÀûÁß·ü)Down: %d Down Hit: %d Down Not Hit : %d Down No Signal : %d\n",
		(double)nDownHit/(nDownNotHit+nDownHit)*100,nDown,nDownHit,nDownNotHit,nDownNoSignal);
	printf("[how future explains option]\n");
	printf("(%lf ÀûÁß·ü)Up: %d Up Hit: %d Up Not Hit : %d Up No Signal : %d\n",
		(double)nPUpHit/(nPUpNotHit+nPUpHit)*100,nPUp,nPUpHit,nPUpNotHit,nPUpNoSignal);
	printf("(%lf ÀûÁß·ü)Down: %d Down Hit: %d Down Not Hit : %d Down No Signal : %d\n",
		(double)nPDownHit/(nPDownHit+nPDownNotHit)*100,nPDown,nPDownHit,nPDownNotHit,nPDownNoSignal);
}

void report_momentum(int start_time, int end_time){
	setup();
#ifdef _DEBUG
	const int unitTimeInterval = 100000;
#else
	const int unitTimeInterval = 1000000;
#endif
	int nDivision = (end_time - start_time) / unitTimeInterval + 1;
	for (int i=0;i<nDivision;++i){
		int fromtime, totime;
		fromtime = ((nDivision - i) * start_time + (i) * end_time) / nDivision;
		totime = ((nDivision - i - 1) * start_time + (i + 1) * end_time) / nDivision;
		setup_time(fromtime, totime);
		printf("time %d to time %d\n",fromtime,totime);
		momentum_report();
	}
}

#include <algorithm>
#include <functional>
#include <numeric>

template<class input_type>
void normalize(std::vector<input_type>& rhs){
	input_type max_value = *(std::max_element(rhs.begin(), rhs.end()));
	if (max_value > 0){
		for (int i=0;i<(int)rhs.size();++i)
			rhs[i] /= max_value;
	}
}

void variance_analysis(int start_time, int end_time){
	setup();
	setup_time(start_time, end_time, ReaderStatic::FUT);
	std::vector<Brick *> fb = ReaderStatic::get().futbase().getAll();
	rolling_statistics<double> rollstats(20);
	std::string targetFuture = "KR4101H60001";

	std::vector<int> timestamps;
	std::vector<double> means, variances, stdevs, norm_stdevs, prices;
	// bid dynamics
	for (int i=0;i<(int)fb.size();++i){
		if (!STRCMPI_FRONT(fb[i]->krcode, targetFuture)){
			rollstats.add(fb[i]->ob.bidquantities[0]);
			timestamps.push_back(fb[i]->pi.timestamp);
			means.push_back(rollstats.mean());
			variances.push_back(rollstats.variance());
			stdevs.push_back(sqrt(variances.back()));
			norm_stdevs.push_back(stdevs.back()/fb[i]->ob.bidquantities[0]);
			prices.push_back(fb[i]->ob.bidprices[0]);
		}
	}
	
	std::vector<int> pricechanges(prices.size(),0);
	int lastTime = end_time + 1000000;
	const int timehorizon = 10; // * 100 ms
	for (int i=(int)prices.size()-2;i>=0;--i){
		if (prices[i] != prices[i+1]){
			lastTime = timestamps[i];
		}
		if (lastTime <= timestamps[i] + timehorizon)
			pricechanges[i] = true;
	}
	double tot_stdev = std::accumulate(norm_stdevs.begin(),norm_stdevs.end(),0.);
	double max_stdev = *(std::max_element(norm_stdevs.begin(),norm_stdevs.end()));
	double avg_stdev = tot_stdev / norm_stdevs.size();
	int nOverAvg = 0, nOAPC = 0, nBelowAvg = 0, nBAPC = 0;
	for (int i=0;i<(int)norm_stdevs.size();++i){
		if (norm_stdevs[i] > avg_stdev){
			++nOverAvg;
			if (pricechanges[i])
				++nOAPC;
		}
		else{
			++nBelowAvg;
			if (pricechanges[i])
				++nBAPC;
		}
	}
	printf("%d %d(%lf%%), %d %d(%lf%%)\n",
		nOverAvg, nOAPC, (double)nOAPC/nOverAvg*100, nBelowAvg, nBAPC, (double)nBAPC/nBelowAvg*100);
	normalize(means);
	normalize(variances);
	/*
	Plotter plotter("A research for volatility-mean price movement");
	plotter.addPlot(std::make_pair(timestamps, means), "Mean", false);
	plotter.addPlot(std::make_pair(timestamps, variances), "Variance", false);
	plotter.refreshForever();
	*/
}

void statistics_test(){
	rolling_statistics<double> rollstats(10);
	printf("incremental ...\n");
	for (int i=0;i<100;++i){
		rollstats.add(i);
		printf("%lf %lf\n",rollstats.mean(), rollstats.variance());
	}
	printf("n^2 + n ...\n");
	for (int i=0;i<100;++i){
		rollstats.add(i*i+i);
		printf("%lf %lf\n",rollstats.mean(), rollstats.variance());
	}
	exit(0);
}

void suite1_plot(){
	setup();
	int start_time = 9100000, end_time = 9595999;
	setup_time(start_time, end_time);
	run(start_time, end_time);
}

void suite2_plot(int scenario_num){
	setup();
	int start_time = 9100000, end_time = 9595999;
	setup_time(start_time, end_time);
	plot_trajectory(start_time, end_time, scenario_num);
}

void suite3_prereport(){
	setup();
	setup_time(11100000, 11290000);
	prereport();
}

void suite4_report_momentum(){
	report_momentum(9100000, 14595999);
}

void suite5_report_variance_signal(){
	variance_analysis(9100000, 9595999);
}

struct traded_pair{
	traded_pair() : quantity(0), dir(0), obi(), obj() {}
	traded_pair(int quantity, int dir, const Orderbook& obi, const Orderbook& obj)
		: quantity(quantity), dir(dir), obi(obi), obj(obj) {}
	int quantity, dir;
	Orderbook obi, obj;
};

void traded_datapair_distribution_analysis(const std::vector<Brick *>& bs1, const std::vector<Brick *>& bs2)
{
	std::vector<Brick *> b1b2;
	std::vector<traded_pair> tpr[2];

	std::map<std::pair<int, int>, int> pricemap[2];
	double currentprices[2] = {0,};
	std::map<std::pair<int, int>, int> price_changed_map[2];
	std::string codes[2];
	Orderbook obs[2];
	codes[0] = bs1[0]->pi.krcodestr; codes[1] = bs2[0]->pi.krcodestr;
	b1b2.insert(b1b2.begin(),bs1.begin(),bs1.end());
	b1b2.insert(b1b2.begin(),bs2.begin(),bs2.end());
	std::sort(b1b2.begin(),b1b2.end(),Functional::ptr_comp<Brick>);
	for (int i=0;i<(int)b1b2.size();++i){
		for (int j=0;j<2;++j){
			if (b1b2[i]->pi.krcodestr == codes[j]){
				obs[j] = b1b2[i]->ob;
				if (obs[j].tradequantity > 0){
					tpr[j].push_back(traded_pair(obs[j].tradequantity,obs[j].dir,obs[j],obs[!j]));
					pricemap[j][std::make_pair((int)obs[j].currentprice, 
						(int)((obs[!j].askprices[0] + obs[!j].bidprices[0])))] += obs[j].tradequantity;
					if (currentprices[j] && obs[j].currentprice != currentprices[j]){
						price_changed_map[j][std::make_pair((int)obs[j].currentprice, 
							(int)((obs[!j].askprices[0] + obs[!j].bidprices[0])))]++;
					}
					currentprices[j] = obs[j].currentprice;
				}
			}
		}
	}
	FILE *fo;
	fopen_s(&fo,"output_distribute.csv","wt");
	for (int m=0;m<2;++m){
		fprintf(fo,"-------------------------------------\n");
		std::map<std::pair<int, int>, int>::const_iterator it = pricemap[m].begin();

		for (;it!=pricemap[m].end();++it){
			fprintf(fo,"%d,%lf,%d\n",it->first.first,(double)it->first.second/2, it->second);
		}
	}

	for (int m=0;m<2;++m){
		fprintf(fo,"-------------------------------------\n");

		std::map<std::pair<int, int>, int>::const_iterator it = price_changed_map[m].begin();

		for (;it!=price_changed_map[m].end();++it){
			fprintf(fo,"%d,%lf,%d\n",it->first.first,(double)it->first.second/2, it->second);
		}

		/*
		for (int i=0;i<(int)tpr[m].size();++i){
			if (tpr[m][i].obj.asktotquantity != 0){
				fprintf(fo,"%lf,%d\n",tpr[m][i].obi.currentprice,
					(tpr[m][i].obj.askprices[0] + tpr[m][i].obj.bidprices[0])/2);
			}
		}
		*/
	}
	fclose(fo);
}

void suite6_tick_traded_distribution(){
	setup();
	setup_time(11100000, 12290000);
	traded_datapair_distribution_analysis
		(ReaderStatic::get().futbase().get("KR4101H60001"),ReaderStatic::get().callbase().get("KR4201H52550"));
}

template <typename T1, typename T2> 
std::pair<T1,T2> operator+(const std::pair<T1,T2> &p1, const std::pair<T1,T2> &p2) { 
	return std::pair<T1,T2>(p1.first + p2.first, p1.second + p2.second); 
} 

void traded_signal_transition_analysis(const std::vector<Brick *>& bs1, const std::vector<Brick *>& bs2)
{
	std::vector<Brick *> b1b2;
	std::map<std::string, int> krcode2idx, krcode2zero;
	std::map<std::pair<std::pair<std::string, std::string>, std::pair<int, int> > , int > transition_cases;
	int n = 0;
	int timeHorizon = 2000; // 2000 msec
	timeHorizon /= 10;

	b1b2.insert(b1b2.begin(),bs1.begin(),bs1.end());
	b1b2.insert(b1b2.begin(),bs2.begin(),bs2.end());
	//std::sort(b1b2.begin(),b1b2.end(),Functional::ptr_comp<Brick>);
	std::sort(b1b2.begin(), b1b2.end(), Functional::ptr_comp_func<Brick>(Brick::rdtscComp) );
	/*
	Functional::ptr_comp_func<Brick> compfunctor(Brick::rdtscComp);
	std::sort(b1b2.begin(), b1b2.end(), compfunctor() );
	*/

	for (int i=0;i<(int)b1b2.size();++i){
		if (krcode2idx.find(b1b2[i]->pi.krcodestr) == krcode2idx.end()){
			krcode2idx[b1b2[i]->pi.krcodestr] = n++;
			krcode2zero[b1b2[i]->pi.krcodestr] = 0;
		}
	}

	for (int i=0;i<(int)b1b2.size();++i){
		Orderbook& ob = b1b2[i]->ob;
		if (ob.tradequantity > 0){
			//printf("%s\n",b1b2[i]->pi.krcodestr.c_str());
			std::map<std::string, int> transitiondir = krcode2zero;
			for (int j=i+1;j<(int)b1b2.size();++j){
				if (b1b2[i]->pi.timestamp + timeHorizon < b1b2[j]->pi.timestamp){
					break;
				}
				if (b1b2[j]->ob.dir != 0){
					transitiondir [b1b2[j]->pi.krcodestr] = b1b2[j]->ob.dir;
					break;
				}
			}
			for (std::map<std::string, int>::const_iterator it = transitiondir.begin(); it != transitiondir.end(); ++it){
				std::string a = b1b2[i]->pi.krcodestr, b = it->first;
				int c = b1b2[i]->ob.dir, d = it->second;
				++transition_cases[std::make_pair(std::make_pair(a,b),std::make_pair(c,d))];
			}
		}
	}
	FILE *fo;
	fopen_s(&fo,"output_transition.csv","wt");
	std::map<std::pair<std::pair<std::string, std::string>, std::pair<int, int> > , int >::const_iterator it = transition_cases.begin();
	//std::map<std::pair<std::pair<std::string, std::string>, int> , int > 

	for (;it!=transition_cases.end();++it){
		fprintf(fo,"%s,%s,%d,%d,%d\n",it->first.first.first.c_str(),it->first.first.second.c_str(),it->first.second.first,it->first.second.second,it->second);
	}
	fclose(fo);
}

void suite7_tick_signal_transition(){
	setup();
#ifdef _DEBUG
	setup_time(11100000, 11190000);
#else
	setup_time(10100000, 11290000);
#endif

	std::vector<std::string> targetCodes;
	targetCodes.push_back("KR4201H52576");
	targetCodes.push_back("KR4201H52600");
	targetCodes.push_back("KR4201H52626");
	targetCodes.push_back("KR4201H52659");

	traded_signal_transition_analysis
		//(ReaderStatic::get().futbase().get("KR4101H60001"),ReaderStatic::get().callbase().get("KR4201H52550"));
		//(ReaderStatic::get().futbase().get("KR4101H60001"),ReaderStatic::get().callbase().getAll());
		(ReaderStatic::get().futbase().get("KR4101H60001"),ReaderStatic::get().callbase().gets(targetCodes));
}

int main(){
	//suite1_plot();
	//suite2_plot(2);
	//suite3_prereport();
	//suite4_report_momentum();
	//suite5_report_variance_signal();
	//suite6_tick_traded_distribution();
	suite7_tick_signal_transition();
	return 0;

	//return 0;

	//statistics_test();
	//int start_time = 9100000, end_time = 9195999;

	//int start_time = 9100000, end_time = 9595999;
	//variance_analysis(9100000, 9595999);

	//setup();
	//setup_time(start_time, end_time);
	//prereport();
	//plot_trajectory(start_time, end_time);
	//run(start_time, end_time);
	return 0;
}
/*
#include <iostream>
#include <windows.h>

long __stdcall WindowProcedure( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
{
    switch(msg)
    {
        case WM_DESTROY:
            std::cout << "\ndestroying window\n" ;
            PostQuitMessage(0) ;
            return 0L ;
        case WM_LBUTTONDOWN:
            std::cout << "\nmouse left button down at (" << LOWORD(lp)
                      << ',' << HIWORD(lp) << ")\n" ;
            // fall thru
        default:
            std::cout << '.' ;
            return DefWindowProc( window, msg, wp, lp ) ;
    }
}

int main()
{
    std::cout << "hello world!\n" ;
    const char* const myclass = "myclass" ;
    WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
                            0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
                            LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
                            0, myclass, LoadIcon(0,IDI_APPLICATION) } ;
    if( RegisterClassEx(&wndclass) )
    {
        HWND window = CreateWindowEx( 0, myclass, "title",
                   WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0 ) ;
        if(window)
        {
            ShowWindow( window, SW_SHOWDEFAULT ) ;
            MSG msg ;
            while( GetMessage( &msg, 0, 0, 0 ) ) DispatchMessage(&msg) ;
        }
    }
}
*/
