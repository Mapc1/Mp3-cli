#include <pulse/simple.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

void printHelp () {
    printf ("Usage: mp3-cli [OPTION]... [FILE]...\nPlays an mp3 file.\n     -h, --help       Display this message\n     -m, --mono       Set audio channel to mono\n     -b=, --bitrate=  Set bitrate\n");
}

time *seconds2Time (double sec) {
    time *time = malloc (sizeof (time));
    
    time->hours = sec / 3600;
    time->minutes = (sec - (3600*time->hours)) / 60;
    time->seconds = (sec - (3600*time->hours) - (time->minutes*60));
    
    return time;
}

void strcpytillEqual (char *src, char *dst) {
    int i;
    int offset;
    for (offset = 0; src[offset - 1] != '='; offset++);
    for (i = 0; src[offset] != '\0'; offset++, i++)
        dst[i] = src[offset];
    dst[i] = '\0';
}

char *getModes (int argc,char *argv[], pa_sample_spec *ss) {
    int i;
    char *filePATH, dup[128];
    i = 1;

    for (; i < argc; i++) {
        if (strcmp (argv[i], "-m") == 0 || strcmp (argv[i], "--mono") == 0){
            ss->channels = 1;
            ss->rate *= 2;
        }
        else if (strncmp (argv[i], "-b=", 3) == 0 || strncmp (argv[i], "--bitrate=", 10) == 0){
            strcpytillEqual (argv[i], dup);
            ss->rate = atoi (dup);
        }
        else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0){
            printHelp ();
            exit (0);
        }
        else if (argv[i][0] != '-')
            filePATH = argv[i];
        else {
            fprintf (stderr, "Error!!!\nUnknown option: %s\n", argv[i]);
            exit (1);
        }
    }

    return filePATH;
}