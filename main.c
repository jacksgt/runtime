/*
 * runtime - a tool to measure the total uptime of a system - by Jack Henschel
 */

/* main.c - core functions */
/* main contains general functions for runtime, such as signal handling, command line arguments and more */

#include "main.h"
#include "print.h"
#include "io.h"
#include "globals.h"

/* This function calculates the amount of digits in a number */
/* takes long int as an argument and returns an int */
/* Thanks! http://stackoverflow.com/a/8257728 */
int calcNumDigits(long int number) {
    return (int)((ceil(log10(number))+1)*sizeof(char));
}

/* this functions saves the runtime, ideally it is called just before shutting down the machine */
/* it reads the values from UPTIME, RUNTIME and RUNTIME_CACHE and adds them toegther */
/* this new value gets written to RUNTIME and RUNTIME_CACHE is cleared */
void saveRuntime() {
    /* read from UPTIME */
    long int uptime = readUptime();
    if (uptime == -1) {
        fprintf(stderr, "Could not open %s\n", UPTIME);
        uptime = 0;
    }

    /* read from RUNTIME */
    long int oldRuntime = readRuntime();

    /* combine values of uptime and old runtime */
    long int runtime = uptime + oldRuntime;

    /* delete cache (the program has been shutdown properly) */
    fclose(fopen(RUNTIME_CACHE, "w"));
    printf("Discarding cache\n");

    printf("Saving runtime %ld\n", runtime);

    writeRuntime(runtime);
}

/* this function is responsible for signal handling */
/* safe signals (with proper shutdowns) are SIGINT and SIGTERM */
_Bool running = 1;
void handleSignal(int sigNum) {
    switch(sigNum) {
    case SIGINT:
    case SIGTERM:
        running = 0;
    default:
        break;
    }
}

void track() {
    /* read previous runtime */
    long int oldRuntime = readRuntime();
    if(oldRuntime == 0)
        printf("Zero seconds read from %s\n", RUNTIME);

    /* read and clear RUNTIME_CACHE */
    long int runtimeCache = readCache();
    if(runtimeCache == 0) {
        printf("Zero seconds read from %s\n", RUNTIME_CACHE);
    } else {
        /* there is still time left in the cache */
        /* append it to RUNTIME and discard RUNTIME_CACHE */
        writeRuntime(oldRuntime+runtimeCache);
        fclose(fopen(RUNTIME_CACHE, "w"));
    }

    /* define exit handler */
    int i = atexit(saveRuntime);
    if(i != 0) {
        fprintf(stderr, "Error setting exit function, aborting\n");
        _exit(EXIT_FAILURE);
    }

    /* define signal handler */
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);

    /* this loop keeps the program alive and wakes it up after CACHE_INTERVAL to run writeCache() */
    do {
        sleep(CACHE_INTERVAL);
        printf("Sleep is over!\n");
        writeCache(CACHE_INTERVAL);
    } while (running);

    // call all exit handlers
    exit(EXIT_SUCCESS);
}


/* main function */
/* responsible for argument handling, defining exit handlers and keeping the program alive */
_Bool verbose = 0;
void main(int argc, char **argv) {
    long int uptime = readUptime();
    if (uptime == -1) {
        fprintf(stderr, "Could not open %s\n", UPTIME);
        uptime = 0;
    }
    long int oldRuntime = readRuntime();

    if(argc >= 2) {

        if(strcmp(argv[1], "--help") == 0) {
            printHelp();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(argv[1], "--verbose") == 0) {
            verbose = 1;
        }

        if(strcmp(argv[1], "--version") == 0) {
            printf("Runtime %s\n", VERSION);
            printf("UPTIME: %s, RUNTIME: %s, RUNTIME_CACHE: %s\n", UPTIME, RUNTIME, RUNTIME_CACHE);
            exit(EXIT_SUCCESS);
        }

        if(strcmp(argv[1], "--save") == 0) {
            printf("Saving runtime now\n");
            saveRuntime();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(argv[1], "--track") == 0) {
            printf("Launching track mode\n");
            track();
        }

        /* displays the current runtime in seconds and exits */
        if(strcmp(argv[1], "--seconds") == 0) {
            printf("%ld\n", uptime + oldRuntime);
            exit(EXIT_SUCCESS);
        }

        /* displays the current uptime */
        if(strcmp(argv[1], "--uptime") == 0) {
            print(uptime);
            exit(EXIT_SUCCESS);
        }

        /* displays the current runtime (the default option) */
        if(strcmp(argv[1], "--runtime") == 0) {
            goto defaultMode;
        }

    }

 defaultMode:
     print(uptime + oldRuntime);
    /* debug: */
     //    printf("uptime: %ld, oldRuntime: %ld, runtimeCache: %ld,  runtime: %ld\n", uptime, oldRuntime, runtimeCache, runtime);

    exit(EXIT_SUCCESS);
}
