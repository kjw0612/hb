#include "obinfo2sig.hpp"
#include "formaldata.hpp"

void formatAdapt(){
	pair<vs, vvd> dp = getDataPool(filepathdatestr_new("20130805"));
	ObDataBase ob(dp.first, dp.second);
	pair<vvd, vvd> xyvec = ob.tset1(10, 50);
	ob.print("D:\\SEOUL_EX_T\\sample.csv");
}
