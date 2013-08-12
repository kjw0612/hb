#include "stdafx.h"
#include "Logger.h"
#include "Config.h"

#include <string>
#include <algorithm>

using namespace std;

Logger::Logger(void)
{
}


Logger::~Logger(void)
{
}

FILE *Logger::flog = NULL;

void Logger::print(std::string str) {
	if (flog == NULL) return;
	print((char*)str.c_str());
}

void Logger::println(std::string str) {
	if (flog == NULL) return;
	print(str + "\n");
}
void Logger::print(char *buf) {
	if (flog == NULL) return;
	fputs(buf, flog);
	fflush(flog);
}
void Logger::println(char *buf) {
	if (flog == NULL) return;
	println( (std::string) buf );
}

void Logger::Initialize() {
	if (flog != NULL) {
		fclose(flog);
		flog = NULL;
	}
	string logPath = Config::Get("LogPath");
	if (logPath.c_str() > 0) {
		fopen_s(&flog, logPath.c_str(), "a");
	}
}