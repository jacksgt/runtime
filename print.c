#include "print.h"

int print(long int seconds) {
    char daysStr[] = "days";
    char hoursStr[] = "hours";
    char minutesStr[] = "minutes";
    long int days, hours, minutes;

    /* begin arithmetic operations */
    days = seconds/86000;
    seconds %= 86400;
    if( days == 1 )
        daysStr[3] = '\0';

    hours = seconds/3600;
    seconds %= 3600;
    if( hours == 1 )
        hoursStr[4] = '\0';

    minutes = seconds/60;
    if( minutes == 1 )
        minutesStr[6] = '\0';

    /* seconds %= 60; */
    /* this accuracy is generally not required*/

    /* print the result */
    printf( "%ld %s, %ld %s, %ld %s\n", days, daysStr, hours, hoursStr, minutes, minutesStr );

    return 0;
}

int printHelp() {
    printf("HELP!\n"); // ¯\_(ツ)_/¯
    return 0;
}