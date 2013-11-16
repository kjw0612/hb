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

	/*
	vvd getforward(const vi& obsdata, const vvd& _obsprob, const vvd& _transprob, int init_state = 0) const {
		int T = obsdata.size();
		vvd alphas(T,vd(n,0)), alphahats(T,vd(n,0));
		vd cs(T,0); double cphi = 1.0;
		for (int t=0;t<T;++t){
			for (int j=0;j<n;++j){
				if (t==0) alphahats[0][j] = _transprob[init_state][j] * _obsprob[j][obsdata[t]];
				else{
					for (int k=0;k<n;++k)
						alphahats[t][j] += alphahats[t-1][k] * _transprob[k][j] * _obsprob[j][obsdata[t]];
				}
				cs[t] += alphahats[t][j];
			}
			cphi *= cs[t];
			for (int j=0;j<n;++j){
				alphahats[t][j] /= cs[t];
				alphas[t][j] = alphahats[t][j] * cphi;
			}
		}
		return alphas;
	}

	vvd getbackward(const vi& obsdata, const vvd& _obsprob, const vvd& _transprob) const {
		int T = obsdata.size();
		vvd betas(T,vd(n,0)), betahats(T,vd(n,0));
		vd cs(T,0); double cphi = 1.0;
		for (int t=T-1;t>=0;--t){
			for (int i=0;i<n;++i){
				if (t==T-1) betahats[t][i] = _transprob[i][n-1];
				else{
					for (int j=0;j<n;++j)
						betahats[t][i] += _transprob[i][j] * _obsprob[j][obsdata[t+1]] * betahats[t+1][j];
				}
				cs[t] += betahats[t][i];
			}
			cphi *= cs[t];
			for (int i=0;i<n;++i){
				betahats[t][i] /= cs[t];
				betas[t][i] = betahats[t][i] * cphi;
			}
		}
		return betas;
	}*/

	vvd getforward(const vi& obsdata, const vvd& _obsprob, const vvd& _transprob, int init_state = 0) const { // 100% confirmed.
		int T = obsdata.size();
		vvd alphas(T,vd(n,0));
		for (int t=0;t<T;++t){
			for (int j=0;j<n;++j){
				if (t==0) alphas[0][j] = _transprob[init_state][j] * _obsprob[j][obsdata[t]];
				else{
					for (int k=0;k<n;++k)
						alphas[t][j] += alphas[t-1][k] * _transprob[k][j] * _obsprob[j][obsdata[t]];
				}
			}
		}
		return alphas;
	}

	vvd getbackward(const vi& obsdata, const vvd& _obsprob, const vvd& _transprob) const { // 100% confirmed.
		int T = obsdata.size();
		vvd betas(T,vd(n,0));
		for (int t=T-1;t>=0;--t){
			for (int i=0;i<n;++i){
				if (t==T-1) betas[t][i] = _transprob[i][n-1];
				else{
					for (int j=0;j<n;++j)
						betas[t][i] += _transprob[i][j] * _obsprob[j][obsdata[t+1]] * betas[t+1][j];
				}
			}
		}
		return betas;
	}

	void calibrate(const vi& obsdata){ // Baum Welch Algorithm..
		int T = obsdata.size();
		obsprob = random_probmat(n,m), transprob = random_probmat(n,n);
		double diff;

		do{
			vvd alphas = getforward(obsdata, obsprob, transprob);
			vvd betas = getbackward(obsdata, obsprob, transprob);
			/*
			for (int i=0;i<T;++i)
				for (int j=0;j<n;++j){
					if (0 <= alphas[i][j] && alphas[i][j] <= 1);
					else printf("error");
					if (0 <= betas[i][j] && betas[i][j] <= 1);
					else printf("error");
				}*/

			//double P_O_given_lambda = 0;
			//for (int j=0;j<n;++j)
			//	P_O_given_lambda += transprob_[0][j] * obsprob_[j][obsdata[0]] * betas[0][j];
			vvd gammas(T,vd(n,0));
			vector<vvd> epsilons(T,vvd(n,vd(n,0)));
			// E step
			for (int t=0;t<T;++t){
				double P_O_given_lambda = 0;
				for (int j=0;j<n;++j) P_O_given_lambda += (alphas[t][j] * betas[t][j]);

				if (P_O_given_lambda == 0) printf("P_O_given_lambda zero!\n");

				double tot_eps = 0;
				for (int j=0;j<n;++j){
					if (t < T-1){
						for (int i=0;i<n;++i)
							tot_eps += alphas[t][i] * transprob[i][j] * obsprob[j][obsdata[t+1]] * betas[t+1][j];
						if (tot_eps == 0) printf("tot_eps zero!\n");
					}
				}

				for (int j=0;j<n;++j){
					gammas[t][j] = (alphas[t][j] * betas[t][j]) /  P_O_given_lambda;
					if (t < T-1){
						for (int i=0;i<n;++i){
							epsilons[t][i][j] = alphas[t][i] * transprob[i][j] * obsprob[j][obsdata[t+1]] * betas[t+1][j] / tot_eps;
							//epsilons[t][i][j] =
							//	alphas[t][i] * transprob_[i][j] * obsprob_[j][obsdata[t+1]] * betas[t+1][j] / alphas[T-1][n-1];
						}
					}
				}
			}
			vvd obsprob_new(n,vd(m,0)), transprob_new(n,vd(n,0));
			// M step
			for (int i=0;i<n;++i){
				double sumtotprob = 0;
				for (int t=0;t<T-1;++t)
					sumtotprob += gammas[t][i];
				for (int j=0;j<n;++j){
					double totprob = 0;
					for (int t=0;t<T-1;++t)
						totprob += epsilons[t][i][j];
					if (totprob>0)
						transprob_new[i][j] = totprob / sumtotprob;
					else
						transprob_new[i][j] = transprob[i][j];
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
					if (sumgamma[j]>0)
						obsprob_new[j][k] = sumgammacond[k][j] / sumgamma[j];
					else
						obsprob_new[j][k] = obsprob[j][k];
				}
			}
			//print(3);
			diff = 0;
			for (int i=0;i<n;++i){
				for (int j=0;j<m;++j){
					diff += fabs(obsprob[i][j]-obsprob_new[i][j]);
					obsprob[i][j] = obsprob_new[i][j];
				}
				for (int j=0;j<n;++j){
					diff += fabs(transprob[i][j]-transprob_new[i][j]);
					transprob[i][j] = transprob_new[i][j];
				}
			}
			printf("diff : %lf\n",diff);
		}while(diff > 0.05); // until converges
		// M step
	}

	void print(int len){
		vvd prob_times(len,vd(n,0));
		prob_times[0] = vd(n,1./n);

		vi obstates(len,0);

		for (int l=0;l<n;++l){
			printf("state %d\n",l);
			for (int i=0;i<(int)pow((double)m,(int)len);++i){
				int permu = i;
				for (int j=0;j<len;++j){
					obstates[j] = permu%m;
					permu = permu / m;
					printf("%d",obstates[j]);
				}

				//getforward(const vi& obsdata, const vvd& _obsprob, const vvd& _transprob)
				vvd res = getforward(obstates, obsprob, transprob, l);
				double tot=0;
				for (int j=0;j<(int)res.back().size();++j){
					tot += res.back()[j];
				}
				printf(" : %lf\n",tot);
				//printf(" : %lf",);
			
				/*
				for (int j=0;j<n;++j){
					for (int k=0;k<n;++k)
						prob_times[i][j] += prob_times[0][j] * transprob[j][k];
				}*/
			}
		}
	}

	vvd transprob;
	vvd obsprob;
	vd initprob;

	int n,m;
};

//, public CostFunction
class HiddenMarkovModel{
public:

	// depth means # of layer on neural network 
	HiddenMarkovModel (int nstates = 2, int mobstates = 2)
		: nstates(nstates), mobstates(mobstates), himpl(nstates, mobstates) {}

	void train_state(const vi& stseq){
		vvi trseqs = splitvec(stseq, 500);
		//for (int i=0;i<(int)trseqs.size();++i){
		for (int i=0;i<1;++i){
			himpl.calibrate(trseqs[i]);
		}
	}

	void test_state(const vi& testseq){
		vvi trseqs = splitvec(testseq, 500);
		for (int i=0;i<(int)trseqs.size();++i){
		}
	}

	template<class T>
	vi raw2states(const vector<T>& seqs){
		map<T, int> states;
		vi stseq(seqs.size());
		int nenc = 0;
		for (int i=0;i<(int)seqs.size();++i){
			if (states.find(seqs[i]) == states.end())
				states[seqs[i]] = nenc++;
			stseq[i] = states[seqs[i]];
		}
		return stseq;
	}

	template<class T>
	void test(const vector<T>& seqs){
		test_state(raw2states(seqs));
	}

	template<class T>
	void train(const vector<T>& seqs){
		train_state(raw2states(seqs));
	}

	void display(int len){
		himpl.print(len);
	}

	vvd xs, ys;
	int nstates, mobstates;
	HMMImpl himpl;
};


#endif // hidden_markov_model_h__
