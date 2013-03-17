/*
 * krx_futures_factory.hpp
 *
 *  Created on: 2013. 3. 11.
 *      Author: koval
 */

#ifndef KRX_FUTURES_FACTORY_HPP_
#define KRX_FUTURES_FACTORY_HPP_

#include <string.h>
#include <memory.h>
#include "krx_futures_info.hpp"
#include <map>
#include <string>
#include <assert.h>

struct MakeKrxFuturesHeader{
	enum Type{
		t_KrxFuturesDesc = 1,
		t_KrxFuturesBestQuotation = 2,
		t_KrxFuturesTrade = 3,
		t_KrxFuturesTradeBestQuotation = 4,
		t_KrxFuturesEnd = 5,
		t_KrxFuturesMarketManage = 6,
		t_KrxFuturesSpace = 7,
		t_KrxFuturesClose = 8,
		t_KrxFuturesDistribute = 9,
		t_KrxFuturesInvestorData = 10,
		t_KrxFuturesUnsettled = 11,
		t_KrxFuturesSettled = 12,
		t_KrxFuturesMarketpriceRecovery = 13,
		t_KrxFuturesPolling = 14,
		t_KrxFuturesOpenMarketManage = 15,
		t_NoType = 16
	};

	~MakeKrxFuturesHeader()
	{
		if (deepCopy){
			delete copiedMsg;
		}
	}

	union Container{
		const KrxFuturesDesc * krxFuturesDesc;
		const KrxFuturesBestQuotation * krxFuturesBestQuotation;
		const KrxFuturesTrade * krxFuturesTrade;
		const KrxFuturesTradeBestQuotation * krxFuturesTradeBestQuotation;
		const KrxFuturesEnd * krxFuturesEnd;
		const KrxFuturesMarketManage * krxFuturesMarketManage;
		const KrxFuturesSpace * krxFuturesSpace;
		const char * msg;
	};

	static std::map<std::string, Type> codeTypeMap_;
	static int headerTypeSize_[16];

	static std::map<std::string, Type> codeTypeMap(){
		if (codeTypeMap_.empty()){
			codeTypeMap_["A0014"] = t_KrxFuturesDesc;
			headerTypeSize_[t_KrxFuturesDesc] = sizeof(KrxFuturesDesc);
			codeTypeMap_["B6014"] = t_KrxFuturesBestQuotation;
			headerTypeSize_[t_KrxFuturesBestQuotation] = sizeof(KrxFuturesBestQuotation);
			codeTypeMap_["A3014"] = t_KrxFuturesTrade;
			headerTypeSize_[t_KrxFuturesTrade] = sizeof(KrxFuturesTrade);
			codeTypeMap_["G7014"] = t_KrxFuturesTradeBestQuotation;
			headerTypeSize_[t_KrxFuturesTradeBestQuotation] = sizeof(KrxFuturesTradeBestQuotation);
			codeTypeMap_["A6014"] = t_KrxFuturesEnd;
			headerTypeSize_[t_KrxFuturesEnd] = sizeof(KrxFuturesEnd);
			codeTypeMap_["A7014"] = t_KrxFuturesMarketManage;
			headerTypeSize_[t_KrxFuturesMarketManage] = sizeof(KrxFuturesMarketManage);
			codeTypeMap_["G9014"] = t_KrxFuturesSpace;
			headerTypeSize_[t_KrxFuturesSpace] = sizeof(KrxFuturesSpace);
			codeTypeMap_["H0014"] = t_KrxFuturesClose;
			headerTypeSize_[t_KrxFuturesClose] = sizeof(KrxFuturesClose);
			codeTypeMap_["O6014"] = t_KrxFuturesDistribute;
			headerTypeSize_[t_KrxFuturesDistribute] = sizeof(KrxFuturesDistribute);
			codeTypeMap_["H1014"] = t_KrxFuturesInvestorData;
			headerTypeSize_[t_KrxFuturesInvestorData] = sizeof(KrxFuturesInvestorData);
			codeTypeMap_["H2014"] = t_KrxFuturesUnsettled;
			headerTypeSize_[t_KrxFuturesUnsettled] = sizeof(KrxFuturesUnsettled);
			codeTypeMap_["H3014"] = t_KrxFuturesSettled;
			headerTypeSize_[t_KrxFuturesSettled] = sizeof(KrxFuturesSettled);
			codeTypeMap_["B2014"] = t_KrxFuturesMarketpriceRecovery;
			headerTypeSize_[t_KrxFuturesMarketpriceRecovery] = sizeof(KrxFuturesMarketpriceRecovery);
			codeTypeMap_["I2014"] = t_KrxFuturesPolling;
			headerTypeSize_[t_KrxFuturesPolling] = sizeof(KrxFuturesPolling);
			codeTypeMap_["M4014"] = t_KrxFuturesOpenMarketManage;
			headerTypeSize_[t_KrxFuturesOpenMarketManage] = sizeof(KrxFuturesOpenMarketManage);

		}
	}


	MakeKrxFuturesHeader(const char* msg, bool deepCopy)
	: msg(msg), deepCopy(deepCopy), type(t_NoType), size(0), copiedMsg(NULL)
	{
		assert(msg[0]>='A' && msg[0] <= 'Z');
		std::string key(msg,msg+5);
		type = codeTypeMap()[key];
		size = headerTypeSize_[type];

		if (deepCopy){
			copiedMsg = new char[size+1];
			memcpy(copiedMsg, msg, size);
			copiedMsg[size] = '\0';
			container.msg = copiedMsg;
		}
		else{
			container.msg = msg;
		}
	}

	Type type;
	bool deepCopy;
	int size;
	char* copiedMsg;
	Container container;
	const char* msg;
};


#endif /* KRX_FUTURES_FACTORY_HPP_ */
