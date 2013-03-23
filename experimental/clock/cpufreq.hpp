/*
 * cpufreq.hpp
 *
 *  Created on: 2013. 3. 22.
 *      Author: koval
 */

#ifndef CPUFREQ_HPP_
#define CPUFREQ_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <pcre.h>

using namespace std;

uint64_t cpufreq()
{
	uint64_t cpuFreq = 0;

	string pattern = "^cpu MHz\\s*:\\s*(\\d+).*(\\d*)";
	const char* pcreErrorStr = NULL;
	int pcreErrorOffset = 0;

	pcre* reCompiled = pcre_compile(pattern.c_str(), PCRE_ANCHORED, &pcreErrorStr,
									&pcreErrorOffset, NULL);
	if (reCompiled == NULL){
		return 0;
	}

	ifstream ifs("/proc/cpuinfo");
	if(ifs.is_open())
	{
		string line;
		int results[10];
		while(ifs.good())
		{
			getline(ifs, line);
			int rc = pcre_exec(reCompiled, 0, line.c_str(), line.length(),
					0, 0, results, 10);
			if (rc < 0)
			{
				continue;
			}

			double cpumhz;
			sscanf((line.substr(10)).c_str()," %lf",&cpumhz);
			cpuFreq = (uint64_t)(cpumhz * 1000000.0);
		}
	}
	ifs.close();
	pcre_free(reCompiled);

	return cpuFreq;
}



#endif /* CPUFREQ_HPP_ */
