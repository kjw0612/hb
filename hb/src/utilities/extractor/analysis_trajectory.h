#ifndef analysis_trajectory_h__
#define analysis_trajectory_h__

#include <vector>
#include "readerstatic.h"

typedef std::vector<int> v_i;
typedef std::vector<double> v_d;

inline std::pair<v_i, v_d> greek_trajectory
(double fut_price0, const std::pair<v_i, v_d>& opt_price, const std::pair<v_i, v_d>& opt_greek)
{
	std::pair<v_i, v_d> ret;
	if (opt_price.first.size() == opt_price.second.size() && 
		opt_greek.first.size() == opt_greek.second.size() &&
		opt_price.first.size() == opt_greek.first.size())
	{
		ret.first = opt_price.first;
		ret.second = v_d(opt_price.second.size());
		double opt_price0 = opt_price.second[0];
		for (int i=0;i<(int)opt_greek.second.size();++i){
			double opt_pricei = opt_price.second[i];
			double opt_deltai = opt_greek.second[i];
			double opt_price_diff = opt_pricei - opt_price0;
			double fut_price_diff = opt_price_diff / opt_deltai;
			ret.second[i] = fut_price_diff + fut_price0;
		}
		return ret;
	}
	else{
		return ret;
	}
}

inline std::pair<std::pair<v_i, v_d>, std::pair<v_i, v_d> > get_opt_greek_series
(const std::string& krcode, char callput, int start_time, int end_time, Brick::GetDataType dataType = Brick::MidPrice)
{
	const std::vector<Brick *>& greeksbricks = ReaderStatic::get().greeksbase().get(krcode);
	const std::vector<Brick *>& optbricks =
		(callput == 'c') ? ReaderStatic::get().callbase().get(krcode) : ReaderStatic::get().putbase().get(krcode);

	std::pair<v_i, v_d> optseries = bricks2MidPriceGrid(optbricks, start_time, end_time, 5000, dataType);
	std::pair<v_i, v_d> greeksseries = bricks2MidPriceGrid(greeksbricks, start_time, end_time, 5000, Brick::Delta);
	return std::make_pair(optseries,greeksseries);
}

inline std::pair<v_i, v_d> get_trajectory
(double fut_price0, const std::string& krcode, char callput, int start_time, int end_time, Brick::GetDataType dataType = Brick::MidPrice)
{
	std::pair<std::pair<v_i, v_d>, std::pair<v_i, v_d> > ogseries = get_opt_greek_series(krcode, callput, start_time, end_time, dataType);
	return greek_trajectory(fut_price0, ogseries.first, ogseries.second);
}

inline std::pair<v_i, v_d> get_synthetic_trajectory(double fut_price0, const std::string& krcode, char callput, 
											 const std::string& krcode_2, char callput_2, int start_time, int end_time,
											 Brick::GetDataType dataType = Brick::MidPrice, Brick::GetDataType dataType2 = Brick::MidPrice)
{
	std::pair<std::pair<v_i, v_d>, std::pair<v_i, v_d> > ogseries1 = get_opt_greek_series(krcode, callput, start_time, end_time, dataType);
	std::pair<std::pair<v_i, v_d>, std::pair<v_i, v_d> > ogseries2 = get_opt_greek_series(krcode_2, callput_2, start_time, end_time, dataType2);
	std::pair<v_i, v_d> syn_opt_series = aXpbY(1, ogseries1.first, -1, ogseries2.first);
	std::pair<v_i, v_d> syn_greeks_series = aXpbY(1, ogseries1.second, -1, ogseries2.second);
	return greek_trajectory(fut_price0, syn_opt_series, syn_greeks_series);
}

#endif // analysis_trajectory_h__