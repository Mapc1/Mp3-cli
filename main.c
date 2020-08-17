#include <stdio.h>
#include <errno.h>
#include <mpg123.h>
#include <pulse/simple.h>
#include <string.h>

void printHelp () {
    printf ("Usage: mp3-cli [OPTION]... [FILE]...\nPlays an mp3 file.\n     -h, --help       Display this message\n     -m, --mono       Set audio channel to mono\n     -b=, --bitrate=  Set bitrate\n");
}

void strcpyOffset (char *src, char *dst, int offset) {
    int i;
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
        else if (strncmp (argv[i], "-b=", 3) == 0){
            strcpyOffset (argv[i], dup, 3);
            ss->rate = atoi (dup);
        } //These need to be split because of 'strcpyOffset'
        else if (strncmp (argv[i], "--bitrate=", 10) == 0) {
            strcpyOffset (argv[i], dup, 10);
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

int main (int argc, const char *argv[]) { 
    char buffer[1024];
    char *filePATH;
    struct mpg123_fmt *format;
    mpg123_handle *handle;
    size_t decoded = 1;
    pa_simple *s;
    pa_sample_spec ss;

    if (argc == 1){
        printHelp ();
        exit (1);
    }
    
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;

    filePATH = getModes (argc, (char **) argv, &ss);

    mpg123_init ();
    handle = mpg123_new (NULL, NULL);

    mpg123_open_fixed (handle, filePATH, MPG123_STEREO, MPG123_ENC_SIGNED_16);
    s = pa_simple_new (NULL, "MP3-cli", PA_STREAM_PLAYBACK, NULL, "Cli mp3 player", &ss, NULL, NULL, NULL);

    mpg123_read (handle, buffer, 1024, &decoded);

    while (decoded > 0) {
        pa_simple_write (s, buffer, decoded, NULL);
        mpg123_read (handle, buffer, 1024, &decoded);
    }
    
Exit:
    return 0;
}