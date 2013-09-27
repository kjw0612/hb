#ifndef optimization_method_h__
#define optimization_method_h__
#include "settings.hpp"
#include "functionals.hpp"

class CostFunction{
public:
	virtual double costFunction(const vd& param) const = 0;
};

class OptimizationMethod{
public:
	virtual void minimize(CostFunction& cf, const vd& param0) = 0;
};

class GradientDescent : public OptimizationMethod{
public:
	GradientDescent(double alpha0 = 0.4, int maxIter = 20, double expDecay = 0.9, double delta = 1e-4)
		: alpha0(alpha0), maxIter(maxIter), expDecay(expDecay), delta(delta) {}

	void minimize(CostFunction& cf, const vd& param0)
	{
		double alpha = alpha0;
		param = param0;
		for (int i=0;i<maxIter;++i){
			printf("[%d] %lf ",i, cf.costFunction(param));
			vd dvdx(param0.size(),0), deltavec(param0.size(),0);
			for (int j=0;j<(int)param.size();++j){
				deltavec[j] = delta;
				double cf_up = cf.costFunction(param+deltavec);
				double cf_down = cf.costFunction(param-deltavec);
				dvdx[j] = (cf_up - cf_down) / (2*delta);
				deltavec[j] = 0;
			}
			param = param - dvdx * alpha;
			alpha = alpha * expDecay;
		}
	}

	vd param;
	double alpha0, expDecay, delta;
	int maxIter;
};


#endif // optimization_method_h__
