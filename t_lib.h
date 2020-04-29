/*
 * types used by thread library
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

//Mine 

// Thread Control Block Node
struct tcb {
    int threadID;
    int threadPriority;
    ucontext_t *threadContext;
    struct tcb *next;
}*tcb;