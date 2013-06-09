#ifndef descset_h__
#define descset_h__
#include "macros.h"
#include "basereaders.h"
#include <map>
#include <algorithm>
#include <string>

struct DescSet{
	struct Desc{
		double strike;
		std::string expirydate;
		char monthtype;
		char callputfut;
	};
	void report_to_file(const std::string& filename){
		FILE *fo = NULL;
		fopen_s(&fo,filename.c_str(), "wt");
		std::map<std::string, Desc>::const_iterator it = descmap.begin();
		for (;it != descmap.end(); ++it){
			fprintf(fo,"%s\t",it->first.c_str());
			fprintf(fo,"%lf\t%c\n",it->second.strike,it->second.callputfut);
		}
		fclose(fo);
	}
	// KrxFuturesDesc = KrxOptionsDesc
	void push(const KrxFuturesDesc *desc)
	{
		char krcode[30] = "";
		COPY_STR(krcode,desc->krcode);
		std::string krcodestr = krcode;
		std::transform(krcodestr.begin(),krcodestr.end(),krcodestr.begin(),::toupper);
		if (descmap.find(krcodestr) == descmap.end()){
			Desc dsc;
			dsc.strike = ATOLL_LEN(desc->strikeprice) / 100000000.; // 999999999.99999999
			char buf[30] = "";
			COPY_STR(buf,desc->expirydate);
			dsc.expirydate = buf;
			dsc.monthtype = desc->monthtype[0];
			switch(krcode[3]){
	case '1':
		dsc.callputfut = 'f';
		break;
	case '2':
		dsc.callputfut = 'c';
		break;
	case '3':
		dsc.callputfut = 'p';
		break;
			}
			descmap[krcodestr] = dsc;
		}
	}
	void append(const DescSet& rhs){
		descmap.insert(rhs.descmap.begin(),rhs.descmap.end());
	}
	std::map<std::string, Desc> descmap;
};

struct MakeDesc{
	//KospiFuturesReader futrdr("data/F191_15571");
	//KospiOptionsReader optrdr("data/C195_15511");
	MakeDesc(const std::string& filepath, char cpf){
		switch(tolower(cpf)){
			case 'f':
				{
					KospiFuturesReader futrdr(filepath);
					while(futrdr.next()){
						KrxFuturesHeader futheader;
						futheader.m_rawmsg = futrdr.content;
						if (futrdr.futuresType==t_KrxFuturesDesc){
							ds.push(futheader.m_KrxFuturesDesc);
						}
					}
					break;
				}
			case 'p':
			case 'c':
			case 'o':
				{
					KospiOptionsReader optrdr(filepath);
					while(optrdr.next()){
						KrxOptionsHeader optheader;
						optheader.m_rawmsg = optrdr.content;
						if (optrdr.optionsType==t_KrxOptionsDesc){
							ds.push((KrxFuturesDesc *)optheader.m_KrxOptionsDesc);
						}
					}
					break;
				}

		}
	}

	DescSet& getDS() {
		return ds;
	}
	DescSet ds;
};

#endif // descset_h__
