#ifndef hidden_markov_model_h__
#define hidden_markov_model_h__

#include "teach_eval_scheme.hpp"
#include "vectorfunctions.hpp"

class HMMImpl{
public:
	HMMImpl(int n, int m) : n(n), m(m), transprob(n,vd(n,0)), obsprob(n,vd(m,0)), initprob(n,0)
	{
		initprob[0] = 1.;
	}

	pair<int, double> estm_next_state(const vi& obs_seq, const vd& init_prob) const { 
		pair<vi, vd> sppair = viterbi(obs_seq, init_prob);
		int last_state = sppair.first.back();
		int next_state = std::max_element(obsprob[last_state].begin(),obsprob[last_state].end()) - obsprob[last_state].begin();
		double next_state_prob = obsprob[last_state][next_state];
		return make_pair(next_state, next_state_prob);
	}

	pair<vi, vd> viterbi(const vi& obs_seq, const vd& init_prob) const { // viterbi's algorithm.
		vvd probs(obs_seq.size(),vd(n,0)); vvi paths(obs_seq.size(),vi(n,0));

		for (int i=0;i<n;++i)
			probs[0][i] = init_prob[i] * transprob[i][obs_seq[0]];

		for (int l=1;l<(int)obs_seq.size();++l){
			for (int i=0;i<n;++i){
				for (int j=0;j<n;++j){
					double rhs = probs[l-1][j] * transprob[j][i] * obsprob[i][obs_seq[l]];
					if (probs[l][i] < rhs){
						probs[l][i] = rhs;
						paths[l][i] = j;
					}
				}
			}
		}
		vi maxseq(n,0); vd maxprob(n,0);
		double pmax = 0; int zt = 0;
		for (int i=0;i<n;++i){
			if (pmax < probs.back()[i]){
				pmax = probs.back()[i];
				zt = i;
			}
		}
		for (int i=n-1;i>=0;--i){
			maxseq[i] = zt;
			maxprob[i] = probs[i][zt];
			zt = paths[i][zt];
		}
		return make_pair(maxseq, maxprob);
	}

	vd random_prob_vec(int n) const{
		double tot = 0; vd ret(n,0);
		for (int i=0;i<n;++i){
			ret[i] = rand();
			tot += ret[i];
		}
		for (int i=0;i<n;++i) ret[i] /= tot;
		return ret;
	}

	vvd random_probmat(int n, int m) const{
		vvd ret(n, vd(m,0));
		for (int i=0;i<n;++i)
			ret[i] = random_prob_vec(m);
		return ret;
	}

	void fitting(const vi& obsdata, int n, int m){ // Baum Welch Algorithm..

		int T = obsdata.size();
		vvd obsprob_ = random_probmat(n,m), transprob_ = random_probmat(n,n);
		double diff;

		do{
			vvd alphas(T,vd(n,0)), betas(T,vd(n,0));
			for (int i=0;i<T;++i){
				int ib = (T-1) - i;
				for (int j=0;j<n;++j){
					if (i==0){
						alphas[0][j] = transprob_[0][j] * obsprob_[j][obsdata[i]];
						betas[ib][j] = transprob_[j][n-1];
					}
					else{
						for (int k=0;k<n;++k){
							alphas[i][j] += alphas[i-1][k] * transprob_[k][j] * obsprob_[j][obsdata[i]];
							betas[ib][j] += betas[ib+1][j] * transprob_[j][k] * obsprob_[k][obsdata[i]];
						}
					}
				}
			}
			double P_O_given_lambda = 0;
			for (int k=0;k<n;++k)
				P_O_given_lambda += transprob_[0][k] * obsprob_[k][obsdata[0]] * betas[0][k];
			vvd gammas(T,vd(n,0));
			vector<vvd> epsilons(T,vvd(n,vd(n,0)));
			// E step
			for (int t=0;t<T;++t){
				for (int j=0;j<n;++j){
					gammas[t][j] = (alphas[t][j] * betas[t][j]) /  P_O_given_lambda;
					if (t < T-1){
						for (int i=0;i<n;++i){
							epsilons[t][i][j] =
								alphas[t][i] * transprob_[i][j] * obsprob_[j][obsdata[t+1]] * betas[t+1][j] / alphas[T-1][n-1];
						}
					}
				}
			}
			vvd obsprob_new(n,vd(m,0)), transprob_new(n,vd(n,0));
			// M step
			for (int t=0;t<n;++t){
				for (int i=0;i<n;++i){
					double totprob = 0;
					for (int j=0;j<n;++j)
						totprob += epsilons[t][i][j];
					for (int j=0;j<n;++j)
						transprob_new[i][j] = epsilons[t][i][j] / totprob;
				}
			}
			vvd sumgammacond(m,vd(n,0));
			vd sumgamma(n,0);
			for (int j=0;j<n;++j){
				for (int t=0;t<T;++t){
					sumgammacond[obsdata[t]][j] += gammas[t][j];
					sumgamma[j] += gammas[t][j];
				}
				for (int k=0;k<m;++k){
					obsprob_new[j][k] = sumgammacond[k][j] / sumgamma[j];
				}
			}
			diff = 0;
			for (int i=0;i<n;++i){
				for (int j=0;j<m;++j){
					diff += fabs(obsprob_[i][j]-obsprob_new[i][j]);
					obsprob_[i][j] = obsprob_new[i][j];
				}
				for (int j=0;j<n;++j){
					diff += fabs(transprob_[i][j]-transprob_new[i][j]);
					transprob_[i][j] = transprob_new[i][j];
				}
			}
		}while(diff < 1e-3); // until converges

		// M step
	}

	vvd transprob;
	vvd obsprob;
	vd initprob;

	int n,m;
};

//, public CostFunction
class HiddenMarkovModel : public Mto1System{
public:

	// depth means # of layer on neural network 
	HiddenMarkovModel (const vs& xnames_, int nn, int nstates = 2, int mobstates = 2)
		: Mto1System(xnames_, nn) , nstates(nstates), mobstates(mobstates)
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
	int nstates, mobstates;
	//HMMImpl himpl;
};


#endif // hidden_markov_model_h__
