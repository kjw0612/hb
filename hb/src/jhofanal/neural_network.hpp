#ifndef neural_network_h__
#define neural_network_h__
#include "teach_eval_scheme.hpp"
#include "vectorfunctions.hpp"

class NeuralNetwork : public Mto1System, public CostFunction{
public:
	// depth means # of layer on neural network 
	NeuralNetwork (const vs& xnames_, int nn = 1, int depth = 4, const vi &_layers = vi(),
		const shared_ptr<VectorFunction>& _g = shared_ptr<VectorFunction>())
		: Mto1System(xnames_, nn), depth(depth), layers(_layers), g(_g)
	{
		int m = nn * xnames_.size();
		if (layers.size()==0){
			layers.push_back(m);
			for (int i=1;i<depth-1;++i){
				layers.push_back(m);
			}
			layers.push_back(1);
		}
		for (int i=1;i<(int)layers.size();++i){
			theta.push_back(vvd(layers[i],vd(layers[i-1]+1)));
		}
	}

	void add(const vi& x, const vi& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	virtual void adds(const vvi& xs_, const vvi& ys_){
		vvi xxs_ = remakeX(xs_), yys_ = remakeY(ys_);
		xs.insert(xs.end(),xxs_.begin(),xxs_.end());
		ys.insert(ys.end(),yys_.begin(),yys_.end());
	}

	double eval(const vi& x, const vd& param) const {
	}
	/*
	double eval(const vi& x, const vd& param) const { }*/
	static vvd fwdPropagation(const vi& _x, const vi& _layers, const v3d& _theta,
		const shared_ptr<VectorFunction>& _g)
	{
		vvd ai;
		for (int i=0;i<(int)_layers.size();++i) ai.push_back(vd(_layers[i]));
		ai[0][0] = 1;
		for (int i=0;i<(int)_x.size();++i) ai[0][i+1] = _x[i];
		
		for (int i=1;i<(int)_layers.size();++i){
			ai[i][0] = 1;
			for (int j=1;j<_layers[i];++j)
				ai[i][j] = _g->operator()(ai[i-1] * _theta[i-1][j]);
		}
		return ai;
	}

	static v3d backPropagation(const vi& _x, const vi& _y, const vi& _layers, const v3d& _theta,
		const shared_ptr<VectorFunction>& _g){
		vvd ai = fwdPropagation(_x, _layers, _theta, _g);
		int n = _layers.size();
		vvd deltas(n);
		deltas[n-1] = (ai.back() - _y);
		for (int i=n-2;i>=1;--i){
			vd thetadelta(_layers[i]);
			vvd theta_trans = transpose(_theta[i]);
			for (int j=0;j<(int)theta_trans.size();++j)
				thetadelta[j] = deltas[i+1] * theta_trans[j];

			vvd gprime = _g->derivative(ai[i]);
			// ignore multivariable relationship
			vd gpflat(gprime.size());
			for (int j=0;i<(int)gpflat.size();++j)
				gpflat[j] = gprime[j][j];

			deltas[i] = batchMultip(thetadelta, gpflat);
		}
		v3d dthetas = _theta;
		for (int l=0;l<n-1;++l){
			for (int i=0;i<(int)dthetas[l].size();++i)
				for (int j=0;j<(int)dthetas[l][i].size();++j)
					dthetas[l][i][j] = ai[l][j] * deltas[l+1][i];
		}
		return dthetas;
	}
	

	int depth;
	v3d theta;
	vvi xs, ys;
	vi layers;
	shared_ptr<VectorFunction> g;
};

#endif // neural_network_h__
