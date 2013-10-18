#ifndef neural_network_h__
#define neural_network_h__
#include "teach_eval_scheme.hpp"
#include "vectorfunctions.hpp"

//, public CostFunction
class NeuralNetwork : public Mto1System{
public:

	// depth means # of layer on neural network 
	NeuralNetwork (const vs& xnames_, int nn = 2, int depth = 2, const vi &_layers = vi(),
		const shared_ptr<VectorFunction>& _g = shared_ptr<VectorFunction>(new LogisticVF()))
		: Mto1System(xnames_, nn), depth(depth), layers(_layers), g(_g)
	{
		int m = nn * xnames_.size();
		if (layers.size()==0){
			layers.push_back(m+1);
			for (int i=1;i<depth-1;++i){
				layers.push_back(m+1);
			}
			layers.push_back(2);
		}
		double eps = 1e-4;
		for (int i=1;i<(int)layers.size();++i){
			theta.push_back(vvd(layers[i]-1,vd(layers[i-1])));
			
			for (int j=0;j<(int)theta.back().size();++j){
				for (int k=0;k<(int)theta.back()[j].size();++k){
					theta.back()[j][k] = (rand()%20001-10000) / 10000. * eps;
				}
			}
		}
	}

	void add(const vd& x, const vd& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	virtual void adds(const vvd& xs_, const vvd& ys_){
		vvd xxs_ = remakeX(xs_), yys_ = remakeY(ys_);
		xs.insert(xs.end(),xxs_.begin(),xxs_.end());
		ys.insert(ys.end(),yys_.begin(),yys_.end());
	}

	void lazyOptimize(){
		//xs.resize(200);
		//ys.resize(200);
		//double alpha = 0.2;
		double alpha = 2000;
		if (!g){
			alpha = 1;
		}
		for (int i=0;i<20;++i){
			v3d dtheta = backPropagation(xs, ys, layers, theta, g);
			/*
			v3d dtheta_crosscheck = diffDeriva(xs, ys, layers, theta, g);
			//dtheta = dtheta_crosscheck;
			for (int j=0;j<(int)dtheta.size();++j){
				for(int k=0;k<(int)dtheta[j].size();++k){
					for (int l=0;l<(int)dtheta[j][k].size();++l){
						printf("[%lf %lf] ",dtheta[j][k][l],dtheta_crosscheck[j][k][l]);
					}
					printf("\n");
				}
			}
			scanf("%*c");*/
			pair<double, int> errs = LearningSystem::errors(xs,ys);
			printf("[%d] %lf ",i+1, errs.first/errs.second);
			theta = theta - dtheta * alpha;
			if (g){
				alpha *= 0.7;
				if (i==0){
					alpha = 10;
				}
			}
			else{
				alpha *= 0.8;
			}
		}
	}

	double expect(const vd& x) const{
		vvd aas = fwdPropagation(x, layers, theta, g);
		return aas.back()[1];
	}

	/*
	double error(const vd& x, const vd& y) {
		vvd aas = fwdPropagation(x, layers, theta, g);
		double yval = (double)hashy(y);
		double _y = aas.back()[1];
		return fabs(yval  - _y);
	}*/
	
	static v3d diffDeriva(const vvd& _xs, const vvd& _ys, const vi& _layers, const v3d& _theta,
		const shared_ptr<VectorFunction>& _g)
	{
		v3d theta = _theta;
		v3d ret = _theta;
		double eps = 1e-4;
		for (int i=0;i<(int)theta.size();++i){
			for (int j=0;j<(int)theta[i].size();++j){
				for (int k=0;k<(int)theta[i][j].size();++k){
					ret[i][j][k] = 0;
					for (int l=0;l<(int)_xs.size();++l){
						theta[i][j][k] += eps;
						vvd upVvd = fwdPropagation(_xs[l], _layers, theta, _g);
						theta[i][j][k] -= 2 *eps;
						vvd downVvd = fwdPropagation(_xs[l], _layers, theta, _g);
						theta[i][j][k] += eps;
						double upV = upVvd.back()[1], downV = downVvd.back()[1];
						double upError = upV - hashy(_ys[l]), downError = downV - hashy(_ys[l]);
						ret[i][j][k] += (upError*upError - downError*downError) / (eps);
					}
					ret[i][j][k] /= _xs.size();
				}
			}
		}
		return ret;
	}
	/*
	double eval(const vi& x, const vd& param) const {
		v3d theta = param2Theta(param);
	}*/
	/*
	double eval(const vi& x, const vd& param) const { }*/
	static vvd fwdPropagation(const vd& _x, const vi& _layers, const v3d& _theta,
		const shared_ptr<VectorFunction>& _g)
	{
		vvd ai;
		for (int i=0;i<(int)_layers.size();++i) ai.push_back(vd(_layers[i]));
		ai[0][0] = 1;
		for (int i=0;i<(int)_x.size();++i) ai[0][i+1] = _x[i];
		
		for (int i=1;i<(int)_layers.size();++i){
			ai[i][0] = 1;
			for (int j=1;j<_layers[i];++j){
				ai[i][j] = ai[i-1] * _theta[i-1][j-1];
				if (_g)
					ai[i][j] = _g->operator()(ai[i][j]);
			}
			
		}
		return ai;
	}

	static v3d backPropagation(const vvd& _xs, const vvd& _ys, const vi& _layers, const v3d& _theta,
		const shared_ptr<VectorFunction>& _g)
	{
		v3d dthetas = _theta;
		for (int i=0;i<(int)dthetas.size();++i){
			for (int j=0;j<(int)dthetas[i].size();++j)
				for (int k=0;k<(int)dthetas[i][j].size();++k)
					dthetas[i][j][k] = 0;
		}
		
		for (int ix=0;ix<(int)_xs.size();++ix){
			vd _x = _xs[ix], _y = _ys[ix];
			vvd ai = fwdPropagation(_x, _layers, _theta, _g);
			int n = _layers.size();
			vvd deltas(n);
			double ytarget = hashy(_y);
			deltas[n-1] = (ai.back() - vd(2,ytarget) );
			for (int i=n-2;i>=1;--i){
				vd thetadelta(_layers[i]);
				vvd theta_trans = transpose(_theta[i]);
				for (int j=0;j<(int)theta_trans.size();++j)
					thetadelta[j] = theta_trans[j] * (deltas[i+1]+1);

				vd gpflat(ai[i].size(),1);
				if (_g){
					// TODO : normalize this.
					//_g->derivative(ai[i]);
					// ignore multivariable relationship
					for (int j=0;j<(int)gpflat.size();++j){
						gpflat[j] = ai[i][j] * (1-ai[i][j]);
						//gpflat[j] = gprime[j][j];
					}
					deltas[i] = batchMultip(thetadelta, gpflat);
				}
				else{
					deltas[i] = thetadelta;
				}
			}
			for (int l=0;l<=n-2;++l){
				for (int i=0;i<(int)dthetas[l].size();++i)
					for (int j=0;j<(int)dthetas[l][i].size();++j)
						dthetas[l][i][j] += ai[l][j] * deltas[l+1][i+1];
			}
		}
		for (int i=0;i<(int)dthetas.size();++i){
			for (int j=0;j<(int)dthetas[i].size();++j)
				for (int k=0;k<(int)dthetas[i][j].size();++k)
					dthetas[i][j][k] /= _xs.size();
		}
		return dthetas;
	}
	

	int depth;
	v3d theta;
	vvd xs, ys;
	vi layers;
	shared_ptr<VectorFunction> g;
};

#endif // neural_network_h__
