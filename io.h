#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long int readUptime();

long int readRuntime();

int writeRuntime(long int seconds);

int writeCache(long int seconds);

int readCache();

#endif
