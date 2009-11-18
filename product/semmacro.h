#ifndef SEMMACRO_H
#define SEMMACRO_H

#ifdef _SOLARIS

#define sem_t  sema_t
#define sem_init(A, B, C)   sema_init(A, C, USYNC_THREAD, 0)
#define sem_wait(A)         sema_wait(A)
#define sem_post(A)         sema_post(A)
#define sem_destroy(A)      sema_destroy(A)

#endif

#endif
