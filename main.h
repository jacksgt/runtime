#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "globals.h"

int calcNumDigits(long int number);

void saveRuntime();

void handleSignal(int sigNum);

void track();

void main(int argc, char **argv);

#endif
