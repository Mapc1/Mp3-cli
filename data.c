#include <stdio.h>
#include <errno.h>
#include <mpg123.h>
#include <pulse/simple.h>
#include <string.h>
#include <dirent.h>
#include "data.h"

void prependstr (char *fst, char *snd, char *dest){
    int destI, sndI;
    for (destI = 0; fst[destI] != '\0'; destI++)
        dest[destI] = fst[destI];
    for (sndI = 0; snd[sndI] != '\0'; sndI++, destI++)
        dest[destI] = snd[sndI];
}

void initfilePATH (char filePATH[5000][1024]){
    for (int index = 0; index < 5000; index++)
        filePATH[index][0] = '\0';
}

void printHelp () {
    printf ("Usage: mp3-cli [OPTION]... [FILE]...\nPlays an mp3 file.\n     -h, --help       Display this message\n     -m, --mono       Set audio channel to mono\n     -b=, --bitrate=  Set bitrate\n");
}

void strcpyEqual (char *src, char *dst) {
    int i;
    int offset;
    for (offset = 0; src[offset - 1] != '='; offset++);
    for (i = 0; src[offset] != '\0'; offset++, i++)
        dst[i] = src[offset];
    dst[i] = '\0';
}

void getModes (int argc,char *argv[], pa_sample_spec *ss, char filePATH[5000][1024]) {
    int i;
    char dup[1024];
    DIR *directory;
    struct dirent *de;
    i = 1;

    for (; i < argc; i++) {
        if (strcmp (argv[i], "-m") == 0 || strcmp (argv[i], "--mono") == 0){
            ss->channels = 1;
            ss->rate *= 2;
        }
        else if (strncmp (argv[i], "-b=", 3) == 0 || strncmp (argv[i], "--bitrate=", 10) == 0){
            strcpyEqual (argv[i], dup);
            ss->rate = atoi (dup);
        }
        else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0){
            printHelp ();
            exit (0);
        }
        else if (strncmp (argv[i], "-p=", 3) == 0 || strncmp (argv[i], "--playlist=", 11) == 0){
            strcpyEqual (argv[i], dup);
            directory = opendir (dup);
            for (int index = 0; (de = readdir (directory)) != NULL; index++)
                prependstr (dup, de->d_name, filePATH[index]); 
        }
        else if (argv[i][0] != '-')
            strcpy (filePATH[0], argv[i]);
        else {
            fprintf (stderr, "Error!!!\nUnknown option: %s\n", argv[i]);
            exit (1);
        }
    }
}