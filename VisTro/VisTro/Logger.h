#pragma once
#define LOGGING

#ifdef LOGGING 
#define log_println(a) Logger::println(a)
#define log_print(a) Logger::print(a)
#else
#define log_println(a) 
#define log_print(a) 
#endif


#include <string>
class Logger
{
public:
	Logger(void);
	~Logger(void);
	static void print(std::string str);
	static void println(std::string str);
	static void print(char *buf);
	static void println(char *buf);

	static void Initialize();

	static FILE *flog;
};

