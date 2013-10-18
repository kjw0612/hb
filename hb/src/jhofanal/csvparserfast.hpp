#ifndef csvparserfast_h__
#define csvparserfast_h__

#include <string>
#include <vector>
#include <sstream>
#include "functionals.hpp"
#include "fgetsfast/fgetsfast.hpp"

template<class T>
struct FakeFastVector{
	FakeFastVector(int n = 100) : arr(new T[n]), c(0) {}
	~FakeFastVector() { delete[] arr; }
	unsigned int size() const {
		return c;
	}
	void clear(){
		c = 0;
	}
	void reserve(int fakerhs){ }
	inline void push_back(const T& rhs){
		arr[c++] = rhs;
	}
	inline const T& operator[](int i) const {
		//assert(i < c);
		return arr[i];
	}
	inline T& operator[](int i) {
		//assert(i < c);
		return arr[i];
	}
	int c;
	T *arr;
};

typedef FakeFastVector<int> fvi;
typedef FakeFastVector<string> fvs;
typedef FakeFastVector<double> fvd;

class CsvParserFast{
public:
	const int bufsize;
	CsvParserFast(const std::string& __filename) : fp(NULL), bufsize(800) {
		fopen_s(&fp,__filename.c_str(),"rt");
		buf = new char[bufsize+1];
		fgetsfastinit();
	}
	~CsvParserFast() {
		fclose(fp);
		delete[] buf;
	}

	int getrow(int isstr = 0){
		c=0;
		if (isstr){
			line.clear();
			line.reserve(20);
		}
		else{
			lined.clear();
			lined.reserve(20);
			offsets.clear();
			offsets.reserve(20);
		}

		memset(buf,0,sizeof(buf));
		fgets(buf,bufsize,fp);
		//fgetsfast(buf,bufsize,fp); // not so fast...sorry.
		if (isstr){
			str = buf;
			std::istringstream iss(str);
			while (std::getline(iss,substr,',')){
				line.push_back(substr);
			}
			return line.size();
		}
		else{
			int offset = 0, len = strlen(buf);
			double val = 0;
			if (len){
				for (int i=0;i<=len;++i){
					if (buf[i]==',' || i==len){
						buf[i] = 0;
						//if (atof(buf+offset) != atofast(buf+offset)) printf("error");
						val = atofast(buf+offset);
						lined.push_back(val);
						offsets.push_back(offset);
						offset = i+1;
					}
				}
			}
			return lined.size();
		}
		return false;
	}

	template<class T1>
	CsvParserFast& operator>> (T1& val)
	{
		if (c < (int)line.size()){
			std::istringstream iss(line[c]); // kufcing ineffective.
			iss >> val;
			++c;
		}
		return (*this);
	}

	bool empty(){
		return !(c < (int)line.size());
	}

	void gets(std::string& rhs){
		if (c < (int)line.size()){
			rhs = line[c++];
		}
	}

	std::string getstr(){
		if (c < (int)line.size()){
			return line[c++];
		}
		return "";
	}

	int c;
	std::vector<std::string> line;
	std::vector<double> lined;
	std::vector<int> offsets;
	//fvs line;
	//fvd lined;
	//fvi offsets;
	std::string substr;
	std::string str;
	FILE *fp;
	bool parsed;
	char *buf;
};

#endif // csvparserfast_h__