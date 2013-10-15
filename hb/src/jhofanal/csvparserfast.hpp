#ifndef csvparserfast_h__
#define csvparserfast_h__

#include <string>
#include <vector>
#include <sstream>

class CsvParserFast{
public:
	CsvParserFast(const std::string& __filename) : fp(NULL) {
		fopen_s(&fp,__filename.c_str(),"rt");
		buf = new char[800+1];
	}
	~CsvParserFast() {
		fclose(fp);
		delete[] buf;
	}

	int getline(int isstr = 0){
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
		fgets(buf,800,fp);
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
						val = atof(buf+offset);
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
	std::string substr;
	std::string str;
	FILE *fp;
	bool parsed;
	char *buf;
};

#endif // csvparserfast_h__