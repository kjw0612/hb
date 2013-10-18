#include <stdio.h>
#include "teach_eval_scheme.hpp"
#include "simple_stat_system.hpp"
#include "simple_linparam_model.hpp"
#include "simple_poly_model.hpp"
#include "logistic_model.hpp"
#include "neural_network.hpp"

string dates[19] = {"20130805", "20130806", "20130807", "20130808", "20130809", 
					"20130812", "20130813", "20130814", "20130816", "20130819",
					"20130820", "20130821", "20130822", "20130823", "20130826",
					"20130827", "20130828", "20130829", "20130830"};


void neural_network_test(){
	vvd xs, ys;
	for (int i=0;i<1000;++i){
		vd x(2), y(1);
		x[0] = rand()%2;
		x[1] = rand()%2;
		y[0] = x[0] && x[1];
		xs.push_back(x), ys.push_back(y);
	}
	shared_ptr<LearningSystem> nnwk(new NeuralNetwork(vs(2),1));
	nnwk->adds(xs, ys);
	nnwk->optimize();
	pair<double, int> res = nnwk->errors(xs,ys);
	printf("\n%lf %d\n",res.first, res.second);
	//scanf("%*c");
}

inline void test_and_report(const string& name,
							const shared_ptr<LearningSystem>& ls, const shared_ptr<TeachEvalScheme>& tescheme)
{
	printf("[%s]\n",name.c_str());
	tescheme->teach(ls);
	tescheme->eval(ls);
	printf("\n%s training set error = %lf\ntest set error = %lf\n\n", name.c_str(), tescheme->errors[0],tescheme->errors[1]);
}

int test(){
	//neural_network_test();
	functionals_test();
	shared_ptr<TeachEvalScheme> tescheme(new TeachEvalScheme());
	/*
	for (int i=0;i<14;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Training);
	for (int i=14;i<19;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Test);
		*/
	int ntrainset = 1;

#ifdef _DEBUG
	ntrainset = 1;
#endif
	for (int i=0;i<ntrainset;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Training);
	for (int i=15;i<16;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Test);

	test_and_report("SimpleStat",
		shared_ptr<LearningSystem> (new SimpleStatSystem(bamnames(),4)), tescheme);

	test_and_report("SimpleStat",
		shared_ptr<LearningSystem> (new SimpleStatSystem(bamnames(),1)), tescheme);
	test_and_report("SimpleStat",
		shared_ptr<LearningSystem> (new SimpleStatSystem(bamnames(),2)), tescheme);
	test_and_report("SimpleStat",
		shared_ptr<LearningSystem> (new SimpleStatSystem(bamnames(),3)), tescheme);

	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(bamnames(),1)), tescheme);
	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(bamnames(),7)), tescheme);
	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(bamnames(),5)), tescheme);
	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(bamnames(),4)), tescheme);
	test_and_report("SimpleLinParam",
		shared_ptr<LearningSystem> (new SimpleLinparamModel(bamnames(),4)), tescheme);
	return 0;

	/*
	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(concat(sbnames(),bamnames()), 3, 2, vi(), shared_ptr<VectorFunction>())), tescheme);
		*/

	/*
	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(qtynames(),3)), tescheme);
		*/

	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(concat(qtynames(),concat(sbnames(),bamnames())),5)), tescheme);

	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(concat(sbnames(),bamnames()),5)), tescheme);

	//test_and_report("NeuralNetwork",
	//	shared_ptr<LearningSystem> (new NeuralNetwork(concat(qtynames(),concat(sbnames(),bamnames())),4)), tescheme);

	//test_and_report("NeuralNetwork",
	//	shared_ptr<LearningSystem> (new NeuralNetwork(concat(bainames(),bacnames()),2)), tescheme);

	//test_and_report("LogisticTraining",
	//	shared_ptr<LearningSystem> (new LogisticModel(concat(sbnames(),bamnames()),4)), tescheme);

	test_and_report("SimpleLinParam",
		shared_ptr<LearningSystem> (new SimpleLinparamModel(concat(sbnames(),bamnames()),4)), tescheme);

	test_and_report("SimpleLinParam",
		shared_ptr<LearningSystem> (new SimpleLinparamModel(concat(qtynames(),concat(sbnames(),bamnames())),4)), tescheme);

	//test_and_report("SimplePoly",
	//	shared_ptr<LearningSystem> (new SimplePolyModel(concat(sbnames(),bamnames()),4)), tescheme);

	test_and_report("SimpleStat",
		shared_ptr<LearningSystem> (new SimpleStatSystem(concat(sbnames(),bamnames()),4)), tescheme);

	return 0;
}
