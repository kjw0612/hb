#ifndef neural_network_h__
#define neural_network_h__

class NeuralNetwork : public Mto1System{
public:
	// depth means # of layer on neural network 
	NeuralNetwork (const vs& xnames_, int nn = 1, int depth = 4) : Mto1System(xnames_, nn), depth(depth),
		// depth, j_{depth+1}, i_{depth}
		theta(depth-1,vvd(nn+1,vvd(nn+1,0))) {}

	void add(const vi& x, const vi& y){
		xs.push_back(x); ys.push_back(y); // lazy evaluation
	}
	virtual void adds(const vvi& xs_, const vvi& ys_){
		vvi xxs_ = remakeX(xs_), yys_ = remakeY(ys_);
		xs.insert(xs.end(),xxs_.begin(),xxs_.end());
		ys.insert(ys.end(),yys_.begin(),yys_.end());
	}

	double eval(const vi& x, const vd& param) const {
		int m = x.size()+1;
		vvd ai(depth,vd(m,0));
		ai[0].insert(ai[0].end(),x.begin(),x.end());
		for (int i=0;i<depth-1;++i){
			ai[i+1][0] = 1;
			for (int j=1;j<m;++j)
				ai[i+1][j] = ai[i] * theta[i][j];
		}
		double ret = param[0];
		for (int i=0;i<(int)x.size();++i) ret += param[i+1] * x[i];
		return ret;
	}

	int depth;
	v3d theta;
	vvi xs, ys;
};

#endif // neural_network_h__
