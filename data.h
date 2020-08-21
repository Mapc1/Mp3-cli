#ifndef __DATA_H__
#define __DATA_H__
#include <stdio.h>

void initfilePATH (char filePATH[5000][1024]);

void printHelp ();

void strcpyEqual (char *src, char *dst);

void getModes (int argc,char *argv[], pa_sample_spec *ss, char filePATH[5000][1024]);

#endif //__DATA_H__