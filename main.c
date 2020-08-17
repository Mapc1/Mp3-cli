#include <stdio.h>
#include <errno.h>
#include <mpg123.h>
#include <pulse/simple.h>
#include <string.h>
#include <dirent.h>
#include "data.h"

int main (int argc, const char *argv[]) { 
    char buffer[1024];
    char filePATH[5000][1024];
    char *dirPATH = "/home/marco/Music";
    struct mpg123_fmt *format;
    mpg123_handle *handle;
    size_t decoded = 1;
    pa_simple *s;
    pa_sample_spec ss;

    if (argc == 1){
        printHelp ();
        exit (1);
    }
    
    initfilePATH (filePATH);

    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;
    
    getModes (argc, (char **) argv, &ss, filePATH);

    mpg123_init ();
    handle = mpg123_new (NULL, NULL);
    s = pa_simple_new (NULL, "MP3-cli", PA_STREAM_PLAYBACK, NULL, "Cli mp3 player", &ss, NULL, NULL, NULL);

    for (int index = 0; filePATH[index][0] != '\0'; index++){
        mpg123_open_fixed (handle, filePATH[index], MPG123_STEREO, MPG123_ENC_SIGNED_16);
        mpg123_read (handle, buffer, 1024, &decoded);

        while (decoded > 0) {
            pa_simple_write (s, buffer, decoded, NULL);
            mpg123_read (handle, buffer, 1024, &decoded);
        }
    }
Exit:
    return 0;
}