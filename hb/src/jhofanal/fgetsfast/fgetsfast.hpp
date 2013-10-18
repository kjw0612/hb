#ifndef fgetsfast_h__
#define fgetsfast_h__
#include <stdio.h>

void fgetsfastinit();
int fgetsfast(char *out, int maxn, FILE * fp);

#endif // fgetsfast_h__