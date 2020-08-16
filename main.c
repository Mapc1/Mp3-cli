#include <stdio.h>
#include <errno.h>
#include <mpg123.h>
#include <fmt123.h>
#include <pulse/simple.h>

int main (int argc, const char *argv[]) { 
    char buffer[1024];
    struct mpg123_fmt *format;
    mpg123_handle *handle;
    size_t decoded = 1;
    pa_simple *s;
    pa_sample_spec ss;

    if (argc != 2){
        fprintf (stderr, "Error!\nThis program requires the path of the mp3 files as an argument!");
        goto Exit;
    }
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;

    mpg123_init ();

    handle = mpg123_new (NULL, NULL);
    mpg123_open_fixed (handle, argv[1], MPG123_STEREO, MPG123_ENC_SIGNED_16);
    
    s = pa_simple_new (NULL, "mp3-cli", PA_STREAM_PLAYBACK, NULL, "Audio", &ss, NULL, NULL, NULL);

    while (decoded > 0) {
        mpg123_read (handle, buffer, 1024, &decoded);
        pa_simple_write (s, buffer, decoded, NULL);
    }
    
Exit:
    return 0;
}