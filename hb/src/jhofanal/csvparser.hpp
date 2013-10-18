#ifndef csvparser_h__
#define csvparser_h__

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class CsvParser{
public:
	CsvParser(const std::string& __filename) : fi(__filename.c_str()) {}
	~CsvParser() { fi.close(); }

	int getrow(){
		c=0;
		line.clear();
		line.reserve(20);
		if (std::getline(fi,str)){
			std::istringstream iss(str);
			while (std::getline(iss,substr,',')){
				line.push_back(substr);
			}
			return line.size();
		}
		return false;
	}

	template<class T1>
	CsvParser& operator>> (T1& val)
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
	std::string substr;
	std::string str;
	std::ifstream fi;
	bool parsed;
};

#endif // csvparser_h__
