#include <stdio.h>
#include <time.h>
#include <mpg123.h>
#include <pulse/simple.h>
#include <string.h>
#include "data.h"
#include <locale.h>
#include <curses.h>


void printBar (double curSec, double totalSec) {
    double fill = (curSec / totalSec) * 50; //The bar has 50 characters

    addstr (" [");
    for (int i = 0; i < fill - 1; i++)
        addch (ACS_BLOCK);
    for (; fill < 50; fill++)
        addch (' ');
    addstr ("] ");
}

void printTime (struct tm *curTime, struct tm *totalTime, double curSec, double totalSec) {
    char buffer[10] = "\0";
    if (totalTime->tm_hour != 0){
        if (totalTime->tm_hour > 10){
            sprintf (buffer, curTime->tm_hour < 10 ? "0%d:" : "%d:", curTime->tm_hour);
            addstr (buffer);
        }
        else { 
            sprintf (buffer, "%d:", curTime->tm_hour);
            addstr (buffer);
        }
    }
    if (totalTime->tm_min != 0) {
        if (totalTime->tm_min > 10 || totalTime->tm_hour != 0){
            sprintf (buffer, curTime->tm_min < 10 ? "0%d:" : "%d:", curTime->tm_min);
            addstr (buffer);
        }
        else {
            sprintf (buffer, "%d:", curTime->tm_min);
            addstr (buffer);
        }
    }
    sprintf (buffer, curTime->tm_sec < 10 ? "0%d" : "%d", curTime->tm_sec);
    addstr (buffer);

    printBar (curSec, totalSec);

    if (totalTime->tm_hour != 0){
            sprintf (buffer, "%d:", totalTime->tm_hour);
            addstr (buffer);
    }
    if (totalTime->tm_min != 0){
        if (totalTime->tm_hour != 0){
            sprintf (buffer, totalTime->tm_min < 10 ? "0%d:" : "%d:", totalTime->tm_min);
            addstr (buffer);
        }
        else {
            sprintf (buffer, "%d:", totalTime->tm_min);
            addstr (buffer);
        }
    }
    sprintf (buffer, totalTime->tm_sec < 10 ? "0%d" : "%d", totalTime->tm_sec);
    addstr (buffer);
}

int main (int argc, const char *argv[]) { 
    char buffer[1024];
    char *filePATH;
    mpg123_handle *handle;
    size_t decoded = 1;
    pa_simple *s;
    pa_sample_spec ss;
    off_t currentFrame, totalFrames;
    double frameTime, curSec, totalSec;
    int t = ERR;
    struct tm *curTime = malloc (sizeof (struct tm));
    struct tm *totalTime = malloc (sizeof (struct tm));
    totalFrames = currentFrame = totalSec = curSec = frameTime = 0;

    if (argc == 1){
        printHelp ();
        exit (1);
    }
    setlocale(LC_ALL, "");

    curTime->tm_sec = curTime->tm_min = curTime->tm_hour = curTime->tm_mday = curTime->tm_mon = curTime->tm_year = curTime->tm_wday = curTime->tm_yday = curTime->tm_isdst = 0;
    
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;

    filePATH = getModes (argc, (char **) argv, &ss);

    mpg123_init ();
    handle = mpg123_new (NULL, NULL);

    mpg123_open_fixed (handle, filePATH, MPG123_STEREO, MPG123_ENC_SIGNED_16);
    s = pa_simple_new (NULL, "MP3-cli", PA_STREAM_PLAYBACK, NULL, "Cli mp3 player", &ss, NULL, NULL, NULL);

    mpg123_scan (handle);
    totalFrames = mpg123_framelength (handle);
    frameTime = mpg123_tpf (handle);
    totalSec = (int) (frameTime * totalFrames);
    seconds2Time (totalSec, totalTime);
    
    initscr ();
    cbreak ();
    nonl ();
    noecho ();
    intrflush (stdscr, TRUE);
    keypad (stdscr, TRUE);
    nodelay (stdscr, TRUE);

    while (decoded > 0) {
        mpg123_read (handle, buffer, 1024, &decoded);
        pa_simple_write (s, buffer, decoded, NULL);
        
        currentFrame = mpg123_tellframe (handle);
        curSec = frameTime * currentFrame;
        seconds2Time (curSec, curTime);

        addch ('\r');
        printTime (curTime, totalTime, curSec, totalSec);
        
        t = getch ();
        switch (t){
            case 'q': exit(0);
        }       
    }

    endwin ();
    return 0;
}