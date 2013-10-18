#include "../functionals.hpp"
#include "fgetsfast.hpp"
#include <stdlib.h>

namespace {
	const int N = 50000;
	char buf[N+1];
	int c = 0;
}

void fgetsfastinit(){
	c = 0;
}

int fgetsfast(char *out, int maxn, FILE * fp)
{
	int i=0, len = 0;
	for (i=0;i<maxn;++i){
		if (c == N) c = 0;
		if (c==0){
			len = fread(buf,1,N,fp);
			buf[len] = '\0';
		}
		if (buf[c] == '\0' || buf[c] == '\n'){
			++c;
			break;
		}
		out[i] = buf[c++];
	}
	out[i] = '\0';
	return i;
}

void fgetstest(){
	string filename = filepathdatestr_new("20130805");
	string ofilename = filename + ".new";
	FILE *fp = NULL, *fo = NULL;
	fopen_s(&fp, filename.c_str(),"rt");
	fopen_s(&fo, ofilename.c_str(),"wt");
	char innerbuf[801] = "";
	while(fgetsfast(innerbuf,800,fp)){
		fprintf(fo,"%s\n",innerbuf);
	}
	fclose(fp);
	fclose(fo);
	exit(0);
}