#include <stdio.h>
#include "teach_eval_scheme.hpp"
#include "simple_stat_system.hpp"

string dates[19] = {"20130805", "20130806", "20130807", "20130808", "20130809", 
					"20130812", "20130813", "20130814", "20130816", "20130819",
					"20130820", "20130821", "20130822", "20130823", "20130826",
					"20130827", "20130828", "20130829", "20130830"};

string filepathdatestr(const string& datestr){
	string ret = "D:\\SEOUL_EX_T\\" + datestr + "\\KR4101H90008_" + datestr + ".csv";
	return ret;
}

int test(){
	shared_ptr<TeachEvalScheme> tescheme(new TeachEvalScheme());
	for (int i=0;i<14;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Training);
	for (int i=14;i<19;++i)
		tescheme->addSet(filepathdatestr(dates[i]),TeachEvalScheme::Test);

	shared_ptr<LearningSystem> spls(new SimpleStatSystem(sbnames()));
	tescheme->teach(spls);
	tescheme->eval(spls);
	printf("training set error = %lf\ntest set error = %lf\n", tescheme->errors[0],tescheme->errors[1]);
	return 0;
}
