#include <time.h>

void printHelp ();
void seconds2Time (double sec, struct tm *time);
void strcpytillEqual (char *src, char *dst);
char *getModes (int argc,char *argv[], pa_sample_spec *ss);