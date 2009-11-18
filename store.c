#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "box.h"
#include "store.h"
#include "tractor.h"
#include "clerk.h"

/* Workaround for solaris */
#ifdef _SOLARIS
#include <synch.h>
#include "semmacro.h"
#else
#include <semaphore.h>
#endif


/* Global array to represent all boxes that will be shelved */
box *allBoxes, *boxesOnTractor;
int allBoxCount, boxesOnTractorCount, no_more_boxes;

/* Semaphores */
sem_t box_available, at_tractor, out_of_boxes, getting_box, aisles[2], shelves[2][2];

extern int numClerks;

/**
 * Create the tractor and clerk threads and get the store going 
*/
void doStore()
{
    pthread_t* tractorThread, *clerkThreads;
    
    /* Initialize the boxes array */
    initBoxes();

    /* Initialize all the semaphores */
    initSemaphores();

    /* Create the tractor thread */
    tractorThread = spawnTractorThread();


    /* Create the clerk threads */
    clerkThreads = spawnClerkThreads(numClerks);

    /* Wait for the threads to finish up, tractor first */
    joinThreads(tractorThread, 1);
    joinThreads(clerkThreads, numClerks);
    
    /* Finally, destroy all the semaphores */
    destroySemaphores();
}

/**
 * Initialize all the boxes in the allBox array and randomly
 * assign them to aisles and shelves
*/
void initBoxes()
{
    int i;

    allBoxCount = 0;
    boxesOnTractorCount = 0;
    no_more_boxes = 0;

    /* Allocate space for the array of boxes */
    if (!(allBoxes = malloc(100 * sizeof(box))))
    {
        fprintf(stderr, "Error allocating memory for the boxes array\n");
        exit(1);
    }

    /* Randomly assign an aisle and a shelf to each one */
    srand( (unsigned int) time( NULL ));
    for (i = 0; i < 100; i++)
    {
        allBoxes[i].aisle = rand() % 2;
        allBoxes[i].shelf = rand() % 2;
    }
}


/**
 * Initialize all the semaphores used for the store
*/
void initSemaphores()
{
    int i, j;

    /* box_available, at_tractor_, getting_box, and out_of_boxes */
    if (sem_init(&box_available, 0, 0) == -1)
    {
        fprintf(stderr, "Error initializing semaphore box_available\n");
        exit(1);
    }

    if (sem_init(&at_tractor, 0, 2) == -1)
    {
        fprintf(stderr, "Error initializing semaphore at_tractor\n");
        exit(1);
    }

    if (sem_init(&getting_box, 0, 1) == -1)
    {
        fprintf(stderr, "Error initializing semaphore getting_box\n");
        exit(1);
    }
    
    if (sem_init(&out_of_boxes, 0, 0) == -1)
    {
        fprintf(stderr, "Error initializing semaphore out_of_boxes\n");
        exit(1);
    }

    /* aisles */
    for (i = 0; i < 2; i++)
    {
        if (sem_init(&aisles[i], 0, 2) == -1)
        {
            fprintf(stderr, "Error initializing semaphore aisles[%d]\n", i);
            exit(1);
        }
    }

    /* shelves */
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {

            if (sem_init(&shelves[i][j], 0, 1) == -1)
            {
                fprintf(stderr, "Error initializing semaphore shelves[%d][%d]\n", i, j);
                exit(1);
            }
        }
    }

}


/**
 * Iterate over an array of threads and wait for each one
 * to complete.
 *
 * A better way to do this would be to assign a unique identifier 
 * to every thread and use a callback function as each thread expires.
 * However, for this project, that's a bit overengineered.
 *
 * @param threadArray  an array of pthread_t structs
 * @param threadCount  the number of Threads in the array
*/
void joinThreads(pthread_t* threadArray, int threadCount)
{
    int i;
    for (i = 0; i < threadCount; i++)
    {
        if (pthread_join(threadArray[i], NULL) != 0)
        {
            fprintf(stderr, "Error joining thread #%d\n", i);
            exit(1);
        }
    }
}


/**
 * Clean up all the semaphores
*/
void destroySemaphores()
{
    int i, j;

    /* box_available, at_tractor_, getting_box, and out_of_boxes */
    if (sem_destroy(&box_available) == -1)
    {
        fprintf(stderr, "Error destroying semaphore box_available\n");
        exit(1);
    }

    if (sem_destroy(&at_tractor) == -1)
    {
        fprintf(stderr, "Error destroying semaphore at_tractor\n");
        exit(1);
    }

    if (sem_destroy(&getting_box) == -1)
    {
        fprintf(stderr, "Error destroying semaphore getting_box\n");
        exit(1);
    }

    if (sem_destroy(&out_of_boxes) == -1)
    {
        fprintf(stderr, "Error destroying semaphore out_of_boxes\n");
        exit(1);
    }

    /* aisles */
    for (i = 0; i < 2; i++)
    {
        if (sem_destroy(&aisles[i]) == -1)
        {
            fprintf(stderr, "Error destroying semaphore aisles[%d]\n", i);
            exit(1);
        }
    }

    /* shelves */
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {

            if (sem_destroy(&shelves[i][j]) == -1)
            {
                fprintf(stderr, "Error destroying semaphore shelves[%d][%d]\n", i, j);
                exit(1);
            }
        }
    }

}


/**
 * Create the tractor thread and return a pointer to it
 *
 * @return           the pthread that represents the tractor
*/
pthread_t* spawnTractorThread()
{
    pthread_t* tracThread = malloc(sizeof(pthread_t));

    if (!tracThread)
    {
        fprintf(stderr, "Error allocating space for the tractor thread\n");
        exit(1);
    }

    if (pthread_create(tracThread, NULL, tractorThread, NULL) != 0)
    {
        fprintf(stderr, "Error starting tractor thread\n");
        exit(1);
    }
   
   return tracThread; 
}


/**
 * Initialize an array of clerk threads and return a pointer to it 
 *
 * @return           a pointer to an array of clerk threads 
*/
pthread_t* spawnClerkThreads()
{
    pthread_t* clerkThreads;
    int i, *clerkNumber;

    if (!(clerkThreads = malloc(numClerks * sizeof(pthread_t))))
    {
        fprintf(stderr, "Error allocating space for the %d clerk threads\n", numClerks);
        exit(1);
    }
    
    for (i = 0; i < numClerks; i++)
    {
        if (!(clerkNumber = malloc(sizeof(int))))
        {
            fprintf(stderr, "Error allocating space for the clerk number\n");
            exit(1);
        }

        *clerkNumber = i;
        if (pthread_create(&clerkThreads[i], NULL, clerkThread, (void*) clerkNumber) != 0)
        {
            fprintf(stderr, "Error starting clerk thread %d\n", i);
            exit(1);
        } 
    }

    return clerkThreads;
}

