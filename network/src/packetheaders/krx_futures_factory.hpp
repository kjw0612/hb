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

struct MakeKrxFuturesHeader{
	enum Type{
		t_KrxFuturesDesc,
		t_KrxFuturesBestQuotation,
		t_KrxFuturesTrade,
		t_KrxFuturesTradeBestQuotation,
		t_KrxFuturesEnd,
		t_KrxFuturesMarketManage,
		t_KrxFuturesSpace,
		t_NoType
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


	MakeKrxFuturesHeader(const char* msg, bool deepCopy)
	: msg(msg), deepCopy(deepCopy), type(t_NoType), size(0), copiedMsg(NULL)
	{
		if (!strncmp(msg,"A0014",5)){
			type = t_KrxFuturesDesc; size = sizeof(KrxFuturesDesc);
		}
		else if (!strncmp(msg,"B6014",5)){
			type = t_KrxFuturesBestQuotation;  size = sizeof(KrxFuturesBestQuotation);
		}
		else if (!strncmp(msg,"A3014",5)){
			type = t_KrxFuturesTrade;  size = sizeof(KrxFuturesTrade);
		}
		else if (!strncmp(msg,"G7014",5)){
			type = t_KrxFuturesTradeBestQuotation;  size = sizeof(KrxFuturesTradeBestQuotation);
		}
		else if (!strncmp(msg,"A6014",5)){
			type = t_KrxFuturesEnd;  size = sizeof(KrxFuturesEnd);
		}
		else if (!strncmp(msg,"A7014",5)){
			type = t_KrxFuturesMarketManage;  size = sizeof(KrxFuturesMarketManage);
		}
		else if (!strncmp(msg,"G9014",5)){
			type = t_KrxFuturesSpace;  size = sizeof(KrxFuturesSpace);
		}
		else if (!strncmp(msg,"H0014",5)){
		}
		else if (!strncmp(msg,"O6014",5)){
		}
		else if (!strncmp(msg,"H1014",5)){
		}
		else if (!strncmp(msg,"H2014",5)){
		}
		else if (!strncmp(msg,"H3014",5)){
		}
		else if (!strncmp(msg,"B2014",5)){
		}
		else if (!strncmp(msg,"I2014",5)){
		}
		else if (!strncmp(msg,"M4014",5)){
		}

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
