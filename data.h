typedef struct {
    int hours, minutes, seconds;
} time;

void printHelp ();
time *seconds2Time (double sec);
void strcpytillEqual (char *src, char *dst);
char *getModes (int argc,char *argv[], pa_sample_spec *ss);