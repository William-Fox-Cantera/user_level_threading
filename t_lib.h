/*
 * types used by thread library
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <signal.h>

//Mine 

// Thread Control Block Node
struct tcb {
    int threadID;
    int threadPriority;
    ucontext_t *threadContext;
    struct tcb *next;
};

typedef struct tcb tcb;

// Semaphore Node
typedef struct {
    int count;
    tcb *q;
} sem_t;