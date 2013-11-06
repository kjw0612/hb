#ifndef hidden_markov_model_h__
#define hidden_markov_model_h__

#include "teach_eval_scheme.hpp"
#include "vectorfunctions.hpp"

//, public CostFunction
class HiddenMarkovModel : public Mto1System{
public:

	// depth means # of layer on neural network 
	HiddenMarkovModel (const vs& xnames_, int nn, int nstates = 2, int mobstates = 2)
		: Mto1System(xnames_, nn)
	{}

	void add(const vd& x, const vd& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	virtual void adds(const vvd& xs_, const vvd& ys_){
		vvd xxs_ = remakeX(xs_), yys_ = remakeY(ys_);
		xs.insert(xs.end(),xxs_.begin(),xxs_.end());
		ys.insert(ys.end(),yys_.begin(),yys_.end());
	}

	void lazyOptimize(){
	}

	vvd xs, ys;
};


#endif // hidden_markov_model_h__
