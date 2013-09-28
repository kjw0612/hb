#ifndef VECTOR_FUNCTIONS_HPP
#define VECTOR_FUNCTIONS_HPP

#include "settings.hpp"
#include "functionals.hpp"

class VectorFunction{
public:
	virtual double operator()(const double rhs){ return operator()(vd(1,rhs)).front(); }
	virtual vd operator()(const vd& rhs) = 0;
	virtual vvd derivative(const vd& rhs){
		const double delta = 1e-4;
		vvd ret;
		vd deltavec(rhs.size(),0);
		for (int i=0;i<(int)rhs.size();++i){
			deltavec[i] = delta;
			vd upvec = operator()(rhs+deltavec);
			vd downvec = operator()(rhs-deltavec);
			ret.push_back((upvec - downvec) / (2*delta));
			deltavec[i] = 0;
		}
		return ret;
	}
};

class LogisticVF : public VectorFunction{
public:
	vd operator()(const vd& rhs) {
		vd ret(rhs.size());
		for (int i=0;i<(int)rhs.size();++i)
			ret[i] = logisticFunction(rhs[i]);
		return ret;
	}
	vvd derivative(const vd& rhs){
		vvd ret(rhs.size(),vd(rhs.size(),0));
		for (int i=0;i<(int)rhs.size();++i){
			for (int j=0;j<(int)rhs.size();++j){
				ret[i][j] = logisticDerivative(rhs[i]);
			}
		}
		return ret;
	}
};

#endif // !VECTOR_FUNCTIONS_HPP
