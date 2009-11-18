#include <stdio.h>
#include <stdlib.h>
#include "box.h"

/* Workaround for solaris */
#ifdef _SOLARIS
#include <synch.h>
#include "semmacro.h"
#else
#include <semaphore.h>
#endif

/* These have already been declared in store.c */
extern box *allBoxes, *boxesOnTractor;
extern int numClerks, allBoxCount, boxesOnTractorCount, no_more_boxes;
extern sem_t box_available, getting_box, out_of_boxes;


/**
 * Calculate the difference between two time values in milliseconds
 *
 * @param starttime  the starting time
 * @param endtime    the ending time
 * @return           a string formatted with the time difference
 *                   between starttime and endtime with no units
 *                   Example: "40.384"
*/
void* tractorThread(void* arg)
{
    int i;

    printf("Tractor is created.\n");

    while (allBoxCount < 100)
    {
        /* Fetch boxes from the store room */
        printf("Tractor gets 20 boxes from the stock room.\n");
        get20Boxes();
        printf("Tractor returns from the stock room.\n");

        /* Signal that there's 20 new boxes available for stocking */
        for (i = 0; i < 20; i++)
        {
            sem_post(&box_available);
        }

        /* Wait for a signal from the clerks that we need more boxes */
        sem_wait(&out_of_boxes);
    }


    /* Set the no_more_boxes flag and signal the box semaphore 
     * one time for each clerk. This will ensure that all clerk
     * threads will wake up and know to terminate */
    no_more_boxes = 1;
    for (i = 0; i < numClerks; i++)
    {
        sem_post(&box_available);
    }

    /* We're done */
    printf("Tractor has finished.\n");
    return NULL;
}


/**
 * Copy 20 boxes from the allBoxes array into the tractor's array
 *
 * @return           the number of boxes that were successfully
 *                   pulled into the tractor's box array
*/
int get20Boxes()
{
    int i;

    /* First, test if we've already gotten 100 boxes */
    if (allBoxCount >= 100)
        return 0;

    /* Second, if we haven't yet allocated space for the tractor's
     *  boxes, do so now */
    if (boxesOnTractor == NULL)
    {
        if (!(boxesOnTractor = malloc(20 * sizeof(box))))
        {
            fprintf(stderr, "Error allocating memory for boxes on tractor.\n");
            exit(1);
        }
    }
    
    /* Simulate work being done with a sleep */
    sleep(4);

    /* Copy 20 boxes from the allBox array into the tractor box array */    
    /* The semaphore ensures that no one is monkeying with the tractor
     *  array or boxcount while we're doing this */
    sem_wait(&getting_box);
    for (i = 0; i < 20; i++)
    {
       boxesOnTractor[i] = allBoxes[allBoxCount + i];
    }
    boxesOnTractorCount = 20;
    sem_post(&getting_box);

    /* Advance the allBox counter so we know how many boxes have been
     * shelved already */
    allBoxCount += 20;

    /* Return the new tractor boxes count */
    return boxesOnTractorCount;
}
