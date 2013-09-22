#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <algorithm>
#include "extractorunit.hpp"
//#include "krx_kospi_futures.hpp"

ExtractorUnit unit;

inline std::string get1row(std::istringstream & iss, const std::string& d_wordr){
	std::string wordr = d_wordr;
	std::string ret;
	ret = "";
	while(1){
		ret += wordr;
		wordr = "";
		iss >> wordr;
		if (wordr.length()==0){
			break;
		}
		ret += " ";
	}
	return ret;
}


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
			try{
				unit.proceed();
			}
			catch(const std::exception& e){
				std::cerr << e.what() << std::endl;
			}
		}
		else{
			iss >> delim >> wordr;
			if (word.compare("mode")==0){
				std::transform(wordr.begin(),wordr.end(),wordr.begin(),::tolower);
				if (wordr.compare("headergenerate")==0){
					unit.type = ExtractorUnit::headergenerate; }
				else if (wordr.compare("headergenerateorder")==0){
					unit.type = ExtractorUnit::headergenerateorder; }
				else if (wordr.compare("reconfigure")==0){
					unit.type = ExtractorUnit::reconfigure; }
				else if (wordr.compare("reconfigure_detail")==0){
					unit.type = ExtractorUnit::reconfigure_detail;	}
				else{
					unit.type = ExtractorUnit::reformat;}
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
			else if (word.compare("outputfile")==0){
				unit.outputfile = get1row(iss, wordr);
			}
			else if (word.compare("datafile")==0){
				unit.datapath = "";
				unit.datafile = get1row(iss, wordr);
			}
			else if (word.compare("datapath")==0){
				unit.datafile = "";
				unit.datapath = get1row(iss, wordr);
			}
			else if (word.compare("target_datafile")==0){
				unit.target_datafile = get1row(iss, wordr);
			}
			else if (word.compare("target_date")==0){
				unit.target_date_from = unit.target_date_to = "";
				unit.target_date = wordr;
			}
			else if (word.compare("target_date_from")==0){
				unit.target_date = "";
				unit.target_date_from = wordr;
			}
			else if (word.compare("target_date_to")==0){
				unit.target_date = "";
				unit.target_date_to = wordr;
			}
			else if (word.compare("outputfolder")==0){
				unit.outputfolder = "";
				unit.outputfolder = get1row(iss, wordr);
				if (unit.outputfolder[(int)unit.outputfolder.size()-1]=='\\');
				else{
					unit.outputfolder += '\\';
				}
			}
			else if (word.compare("wanteddesc")==0){
				unit.wanteddescfile = wordr;
			}
			else if (word.compare("name")==0){
				unit.name = wordr;
			}
			else if (word.compare("target_krcode")==0){
				unit.target_krcode = wordr;
			}
		}
	}
	fp.close();
	return 0;
}
