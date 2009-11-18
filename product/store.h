#ifndef PROJ2_H
#define PROJ2_H

void doStore();
void initBoxes();
void initSemaphores();
void destroySemaphores();
pthread_t* spawnTractorThread();
pthread_t* spawnClerkThreads();
void joinThreads(pthread_t* threadArray, int threadCount);

#endif
