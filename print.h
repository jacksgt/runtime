/*
 * runtime - a tool to measure the total uptime of a system - by Jack Henschel
 */

/* print.h - header definitions for print.c */
/* print is responsible for printing to the screen, however other functions may also do that */

#ifndef PRINT_H_INCLUDED
#define PRINT_H_INCLUDED

#include <stdio.h>

int print(long int seconds);

int printHelp();

#endif
