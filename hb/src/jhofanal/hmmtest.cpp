#include "hidden_markov_model.hpp"
#include "formaldata.hpp"
#include "obinfo2sig.hpp"

namespace {
	string datesnew[19] = {"20130805", "20130806", "20130807", "20130808", "20130809", 
		"20130812", "20130813", "20130814", "20130816", "20130819",
		"20130820", "20130821", "20130822", "20130823", "20130826",
		"20130827", "20130828", "20130829", "20130830"};
}

void hmmtest(){
	for (int di=0;di<1;++di){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[di]));
		ObDataBase ob(dp.first, dp.second);
		pair<vi, vi> pmdt = ob.pmseq_strict();
		HiddenMarkovModel hmm(20,2);
		hmm.train(pmdt.second);
		freopen("output.txt","wt",stdout);
		hmm.display(3);
		//pmdt.second;
	}
}