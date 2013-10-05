#ifndef simple_linparam_model_h__
#define simple_linparam_model_h__

#include "teach_eval_scheme.hpp"
#include "functionals.hpp"

/*
template<class T>
vector<T> vector<T>::operator+(const vector<T>& rhs) const {
	return (*this);
}*/

class SimpleLinparamModel : public ParamSystem{
public:
	SimpleLinparamModel (const vs& xnames_, int nn = 1) : ParamSystem(xnames_, nn) {}
	double eval(const vd& x, const vd& param) const {
		double ret = param[0];
		for (int i=0;i<(int)x.size();++i) ret += param[i+1] * x[i];
		return ret;
	}
	void lazyOptimize(){
		paramset = vd(xs[0].size()+1,0);
		GradientDescent gd;
		gd.minimize(*this, paramset);
		paramset = gd.param;
	}
};

#endif // simple_linparam_model_h__
