/*
 * semaphore_wait.c
 *
 * Demonstrate use of semaphores for synchronization.
 *
 * g++ sem1.cpp -lpthread -lrt -o sem1
 *
 */
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

sem_t semaphore;

/*
 * Thread start routine to wait on a semaphore.
 */
void *sem_demo (void *arg)
{
    long num = (long)arg;

    cout << "Thread " << num << " waiting" << endl;
    if (sem_wait (&semaphore) == -1)
    {
	cout << "Wait on semaphore" << endl;
	exit(1);
    }
    
    cout << "Thread " << num << " resuming" << endl;
    return NULL;
}




int main (int argc, char *argv[])
{
    const int NUM_THREADS=10; 
    int thread_count;
    pthread_t sem_waiters[NUM_THREADS];
    int status;


    if (sem_init (&semaphore, 0, 0) == -1)
    {
	cout << "Init semaphore" << endl;
	exit(1);
    }

    /*
     * Create n threads to wait concurrently on the semaphore.
     */
    for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {
        status = pthread_create (
            &sem_waiters[thread_count], NULL,
            sem_demo, (void*)thread_count);
        if (status != 0)
	{
	    cout << "Create thread" << endl;
	    exit(1);
	}
    }

    /*
    * delay parent 2 seconds
    */
    sleep (2);

    /*
     * "Broadcast" the semaphore by repeatedly posting until the
     * count of waiters goes to 0.
     */

     for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {

	cout << "Posting from main" << endl;
        if (sem_post (&semaphore) == -1)
	{
	    cout << "Post semaphore" << endl;
	    exit(1);
	}
    }

    /*
     * Wait for all threads to complete.
     */
    for (thread_count = 0; thread_count < NUM_THREADS; thread_count++) {
        status = pthread_join (sem_waiters[thread_count], NULL);
        if (status != 0)
	{
	    cout << "Join thread" << endl;
	    exit(1);
	}
    }
    return 0;
}


