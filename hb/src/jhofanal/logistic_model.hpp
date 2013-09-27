#ifndef logistic_model_h__
#define logistic_model_h__

#include "teach_eval_scheme.hpp"
#include "functionals.hpp"

/*
template<class T>
vector<T> vector<T>::operator+(const vector<T>& rhs) const {
	return (*this);
}*/

class LogisticModel : public ParamSystem{
public:
	LogisticModel(const vs& xnames_, int nn = 1) : ParamSystem(xnames_, nn) {}
	inline static double logisticFunction(double val){
		return 1 / (1 + exp(-val));
	}
	double eval(const vi& x, const vd& param) const {
		double ret = param[0];
		for (int i=0;i<(int)x.size();++i) ret += param[i+1] * x[i];
		return logisticFunction(ret);
	}
	void lazyOptimize(){
		paramset = vd(xs[0].size()+1,0);
		GradientDescent gd;
		gd.minimize(*this, paramset);
		paramset = gd.param;
	}
};


#endif // logistic_model_h__
