#ifndef simple_linparam_model_h__
#define simple_linparam_model_h__

#include "teach_eval_scheme.hpp"
#include "functionals.hpp"

/*
template<class T>
vector<T> vector<T>::operator+(const vector<T>& rhs) const {
	return (*this);
}*/

class SimpleLinparamModel : public Mto1System{
public:
	SimpleLinparamModel (const vs& xnames_, int nn = 1) : Mto1System(xnames_, nn) {}
	void add(const vi& x, const vi& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	inline static double eval(const vi& x, const vd& as){
		double ret = 0;
		for (int i=0;i<(int)x.size();++i) ret += as[i+1] * x[i];
		return ret;
	}
	inline double error(const vi &x, const vi& y){
		double y_real = (double)hashy(y);
		return fabs(y_real - eval(x, paramset));
	}
	void optimize(){
		xs = remakeX(xs); ys = remakeY(ys);
		paramset = vd(xs[0].size()+1,0);
		const double delta = 1e-4;
		double alpha = 0.2;
		// adopt random optimization
		for (int i=0;i<(int)xs.size();++i){
			int ri = rand32()%xs.size();
			vd dvdx(paramset.size(),0);
			vd deltavec(paramset.size(),0);
			for (int j=0;j<(int)paramset.size();++j){
				deltavec[j] = delta;
				double val_up = eval(xs[ri], paramset+deltavec) - hashy(ys[ri]);
				double val_down = eval(xs[ri], paramset-deltavec) - hashy(ys[ri]);
				dvdx[j] = (val_up - val_down) / delta;
				deltavec[j] = 0;
			}
			paramset = paramset - dvdx * alpha;
		}
	}
	vvi xs, ys;
	vd paramset;
};

#endif // simple_linparam_model_h__
