/*
 * runtime - a tool to measure the total uptime of a system - by Jack Henschel
 */

/* io.c - function bodies */
/* io is reponsible for writing and reading from and to disk */

#include "io.h"
#include "main.h"
#include "globals.h"

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

/* this function reads the previous runtime value from RUNTIME and returns the content as long int (seconds) */
/* it does not report errors, instead it returns a 0 */
long int readRuntime() {
    /* get length of file and seek back */
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char line[size];
    int seconds = 0;

    fp = fopen(RUNTIME, "r");

    if(fp == NULL)
        return 0;

    while(fgets(line, size, fp) != NULL) {
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
    /* get length of file and seek back */
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char line[size];
    int seconds = 0;

    fp = fopen(RUNTIME_CACHE, "r");

    if(fp == NULL)
        return 0;

    while(fgets(line, size, fp) != NULL) {
        printf("%s\n",line);
        seconds += strtol(line,NULL,10);
    }

    fclose(fp);
    return seconds;
}
