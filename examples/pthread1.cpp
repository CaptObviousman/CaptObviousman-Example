#include <iostream>
#include <pthread.h>   
#include <unistd.h>

// The function that runs when the thread is created 
void *print_thread(void*);

using namespace std;

int main()
{
   pthread_t tid;              // thread ID structure 
   char hello[]={"Hello, "};   
   char thread[]={"thread"};

   // print hello 
   cout << hello;

   // Create another thread.  ID is returned in &tid 
   // The last parameter is being passed 
   // to the thread function. 
   pthread_create(&tid, NULL, print_thread, thread);

   // Continue on with the base thread and wait for
   // the thread to finish
   pthread_join(tid, NULL);

   return 0;
}

// The thread function
void *print_thread(void* input)
{
     cout << (char *)input;
     cout << endl;
     return NULL;
}
