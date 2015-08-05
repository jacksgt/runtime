/*
 * runtime - a tool to measure the total uptime of a system - by Jack Henschel
 */

/* io.h - header definitions for io.c */
/* io is responsible for writing and reading from and to disk */

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
