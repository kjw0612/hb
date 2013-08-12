#pragma once
#include <string>
#include <map>
class Config
{
public:
	Config(void);
	~Config(void);

	static std::map< std::string, std::string > data;

	static void Initialize(void);
	static void Set(std::string Key, std::string Value);
	static std::string Get(std::string Key);
};
