#ifndef hasher_generator_hpp
#define hasher_generator_hpp

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "jshash.hpp"


std::string tb = "    ";


namespace hashfunc{
	///* http://www.iamcorean.net/38 */
	//inline unsigned int JSHash(const char * str, int n)
	//{
	//   unsigned int hash = 1315423911;
	//   for(int i = 0; i < n; i++)
	//   {
	//	  hash ^= ((hash << 5) + str[i] + (hash >> 2));
	//   }
	//   return hash;
	//}

	std::string jshash_str(){
		std::ostringstream ostr;
		ostr << "inline unsigned int JSHash(const char * str, int n)" << std::endl
		<<"{" << std::endl
		<< tb << "unsigned int hash = 1315423911;" <<std::endl
		<< tb << "for(int i = 0; i < n; i++)" << std::endl
		<< tb << "{" << std::endl
		<< tb << tb <<"hash ^= ((hash << 5) + str[i] + (hash >> 2));" << std::endl
		<< tb << "}" << std::endl
		<< tb << "return hash;" << std::endl
		<<"}";
		return ostr.str();
	}

	inline unsigned int JSHashStr(const std::string& str, int n = -1)
	{
		if (n==-1)
			n = str.length();
		return JSHash(str.c_str(), n);
	}
}

inline std::string hashergenerator(const std::vector<std::string>& hasher, const std::vector<std::string> hashee,
							const std::string & targetname)
{
	std::ostringstream ostr;
	int nhash = 0;
	std::vector<unsigned int> hashkeys;
	for (int i=0;i<(int)hasher.size();++i){
		if (nhash < (int)hasher[i].length())
			nhash = hasher[i].length();
	}


	std::string enumName = "e" + targetname;

	{ // enum block
		ostr << "enum " << enumName << "{" << std::endl;
		for (int i=0;i<(int)hasher.size();++i){
			hashkeys.push_back(hashfunc::JSHashStr(hasher[i], nhash));
			ostr << tb << "t_" << hashee[i] << " = " << hashkeys[i] << "," 
				<< "// JSHash(" << hasher[i] << "," << nhash << ") " << std::endl;
		}
		ostr << "};" << std::endl;
	}

	ostr << "inline " << enumName << " getType" << targetname << "(const char *str){" << std::endl;
	ostr << tb << "return " << "(" << enumName << ")" << " JSHash(str," << nhash << ");" << std::endl;
	ostr << "}" << std::endl;
	
	/*
	std::cout << "switch(JSHash(str," << nhash << ")){" << std::endl;
	for (int i=0;i<(int)hasher.size();++i){
		std::cout << "    case " << hashkeys[i] << " : " <<
			"return " << "t_" << hashee[i] << ";" << std::endl;
	}
	std::cout << "}" << std::endl;
	*/
	return ostr.str();
}

#endif // !hasher_generator_hpp
