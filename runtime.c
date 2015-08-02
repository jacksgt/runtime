/*
 * 'runtime' monitors and manages the total runtime of the system
 * That is all the uptime combined
 */

#define MAX_LINE_LENGTH 64 /* /proc/uptime can't be read with fseek, there a static value is required */
#define UPTIME "/proc/uptime" /* path to uptime provided by the kernel */
#define RUNTIME "go/var/runtime" /* path where runtime should store its value */
#define RUNTIME_CACHE "go/var/runtime_cache" /* path where runtime should store the cache */
#define CACHE_INTERVAL 30 /* interval at which runtime should cache its value (in seconds) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

FILE *fp; /* file handle to be used on for all files */

/* readUptime() will read the value in UPTIME and return its value as long int (seconds) */
/* if there is an error, it returns -1 */
long int readUptime() {
    char line[MAX_LINE_LENGTH];
    char *token;
    int seconds = 0;

    /* read content of UPTIME (provided by the kernel) */
    fp = fopen (UPTIME, "r");

    if(fp == NULL) {
        return -1;
    }

    /* loop through all lines (in this case only one) */
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {

        /* split the line at the space */
        token = strtok(line, " ");

        /* remove last three characters of the string */
        /* accuracy of ".XX" is not required */
        token[strlen(token)-3] = '\0';

        /* convert token to int */
        seconds = strtol(token,NULL,10);
    }

    /* close the file handle and return long int seconds */
    fclose(fp);
    return seconds;
}

/* This function calculates the amount of digits in a number */
/* takes long int as an argument and returns an int */
/* Thanks! http://stackoverflow.com/a/8257728 */
int calcNumDigits(long int number) {
    return (int)((ceil(log10(number))+1)*sizeof(char));
}

/* this function reads the previous runtime value from RUNTIME and returns the content as long int (seconds) */
/* it does not report errors, instead it returns a 0 */
long int readRuntime() {
    char line[MAX_LINE_LENGTH];
    int seconds = 0;

    fp = fopen(RUNTIME, "r");

    if(fp == NULL)
        return 0;

    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        seconds = strtol(line,NULL,10);
    }

    fclose(fp);
    return seconds;
}

/* this function writes the new runtime value to RUNTIME (the previous content is overwritten) */
/* it returns 0 on success, -1 on error */
int writeRuntime(long int runtime) {
    // convert long int to char array
    char line[calcNumDigits(runtime)+1];
    sprintf(line, "%ld\n", runtime);

    /* open RUNTIME with "write" flag and check for errors */
    fp = fopen(RUNTIME, "w");
    if(fp == NULL)
        return -1;

    /* write string to RUNTIME  and close the file handle*/
    fputs(line, fp);
    fclose(fp);

    return 0;
}

/* this function basically does the same like writeRuntime(), */
/* but it appends to the content of RUNTIME_CACHE, instead of overwriting it */
/* it does also return 0 on success and 1 on error */
int writeCache(long int seconds) {
    // convert long int to char array
    char line[calcNumDigits(seconds)+1];
    sprintf(line, "%ld\n", seconds);

    /* open RUNTIME_CACHE in append mode and check for errors */
    fp = fopen(RUNTIME_CACHE, "a");
    if(fp == NULL)
        return -1;

    /* write string to RUNTIME_CACHE */
    fputs(line, fp);
    fclose(fp);

    return 0;
}

/* this function basically does the same like readRuntime(), */
/* but it reads multiple lines and adds all the value together */
/* before returning the value as long int (seconds) */
int readCache() {
    char line[MAX_LINE_LENGTH];
    int seconds = 0;

    fp = fopen(RUNTIME_CACHE, "r");

    if(fp == NULL)
        return 0;

    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%s\n",line);
        seconds += strtol(line,NULL,10);
    }

    fclose(fp);
    return seconds;
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

/* main function */
/* responsible for argument handling, defining exit handlers and keeping the program alive */

void main() {
    /* read from UPTIME */
    long int uptime = readUptime();
    if (uptime == -1) {
        fprintf(stderr, "Could not open %s\n", UPTIME);
        uptime = 0;
    }

    /* read from RUNTIME */
    long int oldRuntime = readRuntime();
    if (oldRuntime == 0)
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

    /* combine values of uptime and old runtime */
    long int runtime = uptime + oldRuntime + runtimeCache;

    /* debug: */
    printf("uptime: %ld, oldRuntime: %ld, runtimeCache: %ld,  runtime: %ld\n", uptime, oldRuntime, runtimeCache, runtime);

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
