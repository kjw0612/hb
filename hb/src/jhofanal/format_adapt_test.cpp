#include "obinfo2sig.hpp"
#include "formaldata.hpp"
#include "teach_eval_scheme.hpp"
#include "neural_network.hpp"

string datesnew[19] = {"20130805", "20130806", "20130807", "20130808", "20130809", 
	"20130812", "20130813", "20130814", "20130816", "20130819",
	"20130820", "20130821", "20130822", "20130823", "20130826",
	"20130827", "20130828", "20130829", "20130830"};

void test_and_report(const string& name,
					 const shared_ptr<LearningSystem>& ls, const shared_ptr<TeachEvalScheme>& tescheme);

void formatAdapt(){

	shared_ptr<TeachEvalScheme> tescheme(new TeachEvalScheme());
	pair<vs, int> vsnn;

	int m = 10, gap = 50;

	ObDataBase::TSET_TYPE tsetType = ObDataBase::TSET2;

	for (int i=0;i<3;++i){
		pair<vs, vvd> dp = getDataPool(filepathdatestr_new(datesnew[i]));
		ObDataBase ob(dp.first, dp.second);
		pair<vvd, vvd> xyvec = ob.tset1(m, gap, tsetType);
		vsnn = ob.tset1vsnn(m, gap, tsetType);
		if (i<2)
			tescheme->addData(xyvec, TeachEvalScheme::Training);
		else
			tescheme->addData(xyvec, TeachEvalScheme::Test);
	}

	test_and_report("NeuralNetwork",
		shared_ptr<LearningSystem> (new NeuralNetwork(vsnn.first, vsnn.second)), tescheme);

	//tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Training);

	//ob.print("D:\\SEOUL_EX_T\\sample.csv");
}
