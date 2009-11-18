#include <stdio.h>
#include "box.h"
#include "clerk.h"

/* Workaround for solaris */
#ifdef _SOLARIS
#include <synch.h>
#include "semmacro.h"
#else
#include <semaphore.h>
#endif

extern box* boxesOnTractor;
extern int boxesOnTractorCount, no_more_boxes;
extern sem_t box_available, at_tractor, out_of_boxes, getting_box, 
       aisles[2], shelves[2][2];

/**
 * Main thread for the clerk which performs all of its duties
*/
void* clerkThread(void* arg)
{
    int clerkNumber = *((int*) arg) + 1;
    int myCount;
    box myBox;
   
    printf("Clerk %d is created.\n", clerkNumber);

    while (!no_more_boxes)
    {
        
        /* Wait on the tractor to have a box */
        sem_wait(&box_available);

        /* Check to see that there's still boxes to get */
        if (no_more_boxes)
        {
            printf("Clerk %d has finished.\n", clerkNumber);
            return NULL;
        }


        /* If so, wait for an opening at the tractor, then fetch one */
        sem_wait(&at_tractor);
        
        /* This section here ensures that only one thread is modifying the 
         * boxesOnTractorCount variable at a time */
        sem_wait(&getting_box);
        myCount = boxesOnTractorCount--;
        myBox = getBox(myCount - 1);
        sem_post(&getting_box);

        /* If this is the last box, signal the tractor to get more */
        if (myCount == 1)
        {
            printf("Clerk %d takes the last box from the tractor.\n", clerkNumber);
            sem_post(&out_of_boxes);
        }
        else
        {
            printf("Clerk %d gets a box from the tractor.\n", clerkNumber);
        }
        sleep(1);

        

        /* Signal that we're done at the tractor */
        sem_post(&at_tractor);

        /* Now try to shelve it */
        sem_wait(&aisles[myBox.aisle]);
        sem_wait(&shelves[myBox.aisle][myBox.shelf]);

        /* Put the box on the shelf */
        printf("Clerk %d puts box on aisle %d shelf %d.\n", clerkNumber, 
                myBox.aisle + 1, myBox.shelf + 1);
        putBoxOnShelf();

        /* Release our position at the shelf and aisle */
        sem_post(&shelves[myBox.aisle][myBox.shelf]);
 
        sem_post(&aisles[myBox.aisle]);
        printf("Clerk %d leaves aisle %d shelf %d.\n", clerkNumber, 
                myBox.aisle + 1, myBox.shelf + 1);

    }

    printf("Clerk %d has finished.\n", clerkNumber);
    return NULL; 
}


/**
 * Get a box from the boxes array and return it
 * @param box_num    the index of the box to retrieve
 * @return           the box represented by box_num
*/
box getBox(int box_num)
{
    return boxesOnTractor[box_num];
}

/**
 * A blank function to simulate the action of putting
 * a box on a shelf
*/
void putBoxOnShelf()
{
    sleep(2);
}
