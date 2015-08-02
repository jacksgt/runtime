/*
* 'runtime' should monitor and manage the total runtime of the system
* That is all the uptime combined
*/

#define MAX_LINE_LENGTH 64
#define RUNTIME "go/var/runtime"
#define RUNTIME_CACHE "go/var/runtime_cache"
#define UPTIME "/proc/uptime"
#define CACHE_INTERVAL 30

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

FILE *fp;

long int readUptime() { /* will read the value from /proc/uptime and return its value as long int in seconds */
  char line[MAX_LINE_LENGTH];
  char *token;
  int seconds;

  /* read content of /proc/uptime (provided by the kernel) */
  fp = fopen (UPTIME, "r");

  if(fp == NULL) {

    return -1;
  }

  while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {

    /* split the line at the space */
    token = strtok(line, " ");

    /* remove last three characters of the string */
    /* accuracy of ".XX" is not required */
    token[strlen(token)-3] = '\0';

    /* convert token to int */
    seconds = strtol(token,NULL,10);
  }

  fclose(fp);
  return seconds;
}

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

int writeRuntime(long int runtime) {
  // convert long int to char array
  char line[MAX_LINE_LENGTH];
  sprintf(line, "%d\n", runtime);

  /* open RUNTIME and check for errors */
  fp = fopen(RUNTIME, "w");
  if(fp == NULL)
    return -1;

  /* write string to RUNTIME */
  fputs(line, fp);
  fclose(fp);

  return 0;
}


int writeCache(long int seconds) {
    // convert long int to char array
    char line[MAX_LINE_LENGTH];
    sprintf(line, "%d\n", seconds);

    /* open RUNTIME_CACHE in append mode and check for errors */
    fp = fopen(RUNTIME_CACHE, "a");
    if(fp == NULL)
        return -1;

    /* write string to RUNTIME_CACHE */
    fputs(line, fp);
    fclose(fp);

    return 0;
}

int readCache() {
    char line[MAX_LINE_LENGTH];
    int seconds = 0;

    fp = fopen(RUNTIME_CACHE, "r");

    printf("Reading %s;\n", RUNTIME_CACHE);

    if(fp == NULL)
        return 0;

    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%s\n",line);
        seconds += strtol(line,NULL,10);
    }

    printf("Cache %d;\n", seconds);

    fclose(fp);
    return seconds;

}

void saveRuntime() {
    /* read from UPTIME */
    long int uptime = readUptime();
    if (uptime == -1) {
        fprintf(stderr, "Could not open %s\n", UPTIME);
        uptime = 0;
    }

    /* read from RUNTIME */
    long int oldRuntime = readRuntime();
    if (oldRuntime == -1) {
        oldRuntime = 0;
    }

    /* read RUNTIME_CACHE */
    long int runtimeCache = readCache();

    /* combine values of uptime and old runtime */
    long int runtime = uptime + oldRuntime + runtimeCache;

    /* delete cache (it has now been saved) */
    fclose(fopen(RUNTIME_CACHE, "w"));
    printf("Discarding cache\n");

    printf("Saving runtime %d\n", runtime);

    writeRuntime(runtime);
}

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

void main() {
  /* read from UPTIME */
  long int uptime = readUptime();
  if (uptime == -1) {
    fprintf(stderr, "Could not open %s\n", UPTIME);
    uptime = 0;
  }

  /* read from RUNTIME */
  long int oldRuntime = readRuntime();
  if (oldRuntime == -1) {
    fprintf(stderr, "Could not open %s\n", RUNTIME);
    oldRuntime = 0;
  }

  /* read RUNTIME_CACHE */
  long int runtimeCache = readCache();
  if(runtimeCache == 0) {
      printf("Zero seconds read from %s\n", RUNTIME_CACHE);
  }

  /* combine values of uptime and old runtime */
  long int runtime = uptime + oldRuntime + runtimeCache;

  /* debug: */
  printf("uptime: %d, oldRuntime: %d, runtimeCache: %d,  runtime: %d\n", uptime, oldRuntime, runtimeCache, runtime);

  int i = atexit(saveRuntime);
  if(i != 0) {
      fprintf(stderr, "Error setting exit function, aborting\n");
      _exit(EXIT_FAILURE);
  }

  signal(SIGTERM, handleSignal);
  signal(SIGINT, handleSignal);

  do {
      sleep(CACHE_INTERVAL);
      printf("Sleep is over!\n");
      writeCache(CACHE_INTERVAL);
  } while (running);

  // call all exit handlers
  exit(EXIT_SUCCESS);
}
