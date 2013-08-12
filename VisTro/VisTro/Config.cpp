#include "stdafx.h"
#include "Config.h"
#include <string.h>

using namespace std;

std::map< std::string, std::string > Config::data;
Config::Config(void)
{
}


Config::~Config(void)
{
}

void Config::Initialize(void) {
	FILE *in;
	char buf[4096];
	string tmp;
	fopen_s(&in, "config.txt", "r");
	while(!feof(in)) {
		buf[0] = '\0';
		fgets(buf, 4096, in);
		if (buf[0] == '\0') continue;
		size_t len = strlen(buf);

		while (len > 0) {
			char c = buf[len-1];
			if (c != '\n' && c != ' ' && c != '\t' && c != '\r') break;
			buf[--len] = '\0';
		}

		if (buf[0] == '$') {
			for (int i=0; buf[i] != '\0'; i ++) {
				if (buf[i] == '=') {
					buf[i] = '\0';
					Set(buf+1, buf+i+1);
				}
			}
		}
	}
	fclose(in);
}

void Config::Set(string key, string value) {
	data[key] = value;
}

string Config::Get(string key) {
	return data[key];
}