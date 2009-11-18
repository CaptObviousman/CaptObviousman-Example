#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "proj2.h"
#include "store.h"


/* Global to keep track of how many clerks are running around */
int numClerks;

int main()
{
    struct timeval startclock, endclock;

    printf("How many clerks should work in the shop? ");
    scanf("%d", &numClerks);

    if (numClerks < 0)
        return 0;

    gettimeofday(&startclock, NULL);
    doStore();    
    gettimeofday(&endclock, NULL);
    printf("\n%s seconds\n", strTime(startclock, endclock)); 
    return 0;
}

    
/**
 * Calculate the difference between two time values in seconds
 *
 * @param starttime  the starting time
 * @param endtime    the ending time
 * @return           a string formatted with the time difference
 *                   between starttime and endtime with no units
 *                   Example: "40.384"
*/
char* strTime(struct timeval starttime, struct timeval endtime)
{
    char* buffer;
    struct timeval elapsed;


    // Verify that the endtime struct is actually after the starttime
    if (starttime.tv_sec > endtime.tv_sec)
        return NULL;
    if ((starttime.tv_sec == endtime.tv_sec) && (starttime.tv_usec > endtime.tv_usec))
        return NULL;

    // Verify we won't get a negative usec
    if (starttime.tv_usec > endtime.tv_usec)
    {
        endtime.tv_usec += 1000000;
        endtime.tv_sec--;
    }

    // Allocate space for the string
    // Hard set to 30 characters for now, which is enough for 3 x 10^21 years
    buffer = malloc(30 * sizeof(char));

    // If you're out of memory, return a "safe" result
    if (!buffer)
        return NULL;

    // Calculate the elapsed time
    elapsed.tv_sec = endtime.tv_sec - starttime.tv_sec;
    elapsed.tv_usec = endtime.tv_usec - starttime.tv_usec;

    elapsed.tv_usec /= 1000;

    // Produce a formatted string with the given information and return it 
    snprintf(buffer, 30, "%ld.%03ld", elapsed.tv_sec, elapsed.tv_usec);
    return buffer;
}
