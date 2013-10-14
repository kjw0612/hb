#ifndef formaldata_h__
#define formaldata_h__
#include "functionals.hpp"
#include "csvparserfast.hpp"

inline pair<vs, vvd> getDataPool(const string& filename){
	CsvParserFast cp(filename);
	int m = cp.getline(1);

	vs names;
	vvd data;

	data.reserve(200000);
	for (int i=0;i<m;++i)
		names.push_back(ospace::trim(cp.line[i]));
	while (m = cp.getline(0)){
		vd val(names.size());
		for (int j=0;j<(int)names.size();++j){
			val[j] = (int)cp.lined[j];
		}
		data.push_back(val);
	}
	return make_pair(names, data);
}

#endif // formaldata_h__