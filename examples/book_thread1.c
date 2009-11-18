#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* thread_function(void *arg);

int* myNum;

int main()
{
    int res;
    pthread_t thread;
    void* thread_result;
    myNum = malloc(sizeof(int));
    *myNum = 1582;

    res = pthread_create(&thread, NULL, thread_function, (void*) myNum);
    if (res != 0)
    {
        fprintf(stderr, "Thread creation failed\n");
        exit(1);
    }

    printf("Waiting for thread to finish...\n");
    res = pthread_join(thread, &thread_result);
    if (res != 0)
    {
        fprintf(stderr, "Thread join failed\n");
        exit(1);
    }

    printf("Thread joined, it returned %s\n", (char*) thread_result);
    printf("Message is now %d\n", *myNum);

    return 0;
}


void* thread_function(void *arg)
{
    int* theNum = (int*) arg;
    printf("Thread function is running. Argument was %d\n", *theNum);
    sleep(3);
    *theNum = 5000;
    pthread_exit("Thank you for the CPU time!");
}
