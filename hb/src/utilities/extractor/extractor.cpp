#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <algorithm>
#include "extractorunit.hpp"
#include "krx_kospi_futures.hpp"

ExtractorUnit unit;

int main(){
	std::ifstream fp("config.txt");
	std::string str;
	while(std::getline(fp,str)){
		std::istringstream iss(str);
		std::string word, delim, wordr;
		std::transform(word.begin(),word.end(),word.begin(),::tolower);
		iss >> word;
		if (word.compare("end")==0){
			break;
		}
		else if (word.compare("do")==0){
			unit.proceed();
		}
		else{
			iss >> delim >> wordr;
			if (word.compare("mode")==0){
				std::transform(wordr.begin(),wordr.end(),wordr.begin(),::tolower);
				if (wordr.compare("headergenerate")==0){
					unit.type = ExtractorUnit::headergenerate; }
				else{
					unit.type = ExtractorUnit::reformat; }
			}
			else if (word.compare("src")==0){
				unit.srcfile = wordr;
			}
			else if (word.compare("types")==0){
				unit.typesfile = wordr;
			}
			else if (word.compare("object")==0){
				unit.objectfile = wordr;
			}
			else if (word.compare("datafile")==0){
				unit.datafiles.clear();
				while(1){
					unit.datafiles.push_back(wordr);
					wordr = "";
					iss >> wordr;
					if (wordr.length()==0){
						break;
					}
				}
			}
			else if (word.compare("outputfile")==0){
				unit.outputfile = wordr;
			}
			else if (word.compare("wanteddesc")==0){
				unit.wanteddescfile = wordr;
			}
			else if (word.compare("name")==0){
				unit.name = wordr;
			}
		}
	}
	fp.close();
	return 0;
}
