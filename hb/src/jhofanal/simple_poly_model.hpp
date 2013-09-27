#ifndef simple_poly_model_h__
#define simple_poly_model_h__

#include "teach_eval_scheme.hpp"
#include "functionals.hpp"

/*
template<class T>
vector<T> vector<T>::operator+(const vector<T>& rhs) const {
	return (*this);
}*/

class SimplePolyModel : public ParamSystem{
public:
	//m = degree. 1 to simple linear model.
	SimplePolyModel (const vs& xnames_, int nn = 1, int m = 1) : ParamSystem(xnames_, nn), m(m) {}
	double eval(const vi& x, const vd& param) const {
		double ret = param[0];
		for (int j=0;j<m;++j){
			for (int i=0;i<(int)x.size();++i){
				ret += param[i+1 + (j*m)] * pow((double)x[i],(double)j+1);
			}
		}
		return ret;
	}
	void lazyOptimize(){
		paramset = vd((m * xs[0].size())+1,0);
		GradientDescent gd;
		gd.minimize(*this, paramset);
		paramset = gd.param;
	}
	int m;
};

#endif // simple_poly_model_h__
