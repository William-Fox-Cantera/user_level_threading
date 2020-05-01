#include "t_lib.h"

/**
 * This is a user level library for making multi-threaded programs.
 * 
 * @author William Cantera --> (wcantera@udel.edu)
 * @version 0.0.1
 */


struct tcb *running; // Make thread control blocks for the ready and running queues
struct tcb *ready;


/**
 * t_yield, makes the currently running thread relinquish the CPU. It then gets pushed to the end of the ready 
 *          queue and its context is swapped out for the head of the ready queue. The ready queue then moves 
 *          forward.
 * 
 * @param None
 * @return None
 */
void t_yield() {
    if (ready) { // Make sure the ready queue isn't empty
        struct tcb *currentlyRunning, *wantsToRun, **tracker = &ready;
        currentlyRunning = running;
        wantsToRun = ready;
        while(*tracker)
          tracker = &(*tracker)->next;

        *tracker = currentlyRunning;   // Put the currently running thread at the end of the ready list
        ready = ready->next; // Push head forward
        // Make the running queue hold the thread at the head of the ready list
        wantsToRun->next = NULL;
        running = wantsToRun;
        swapcontext(currentlyRunning->threadContext, wantsToRun->threadContext);
    }
}


/**
 * t_init, initialize the context for main. Initializes the running and ready queue's. The running queue is main
 *         while the ready queue is intially NULL.
 * 
 * @param None
 * @return None 
 */
void t_init() {
    struct tcb *temp = (struct tcb *) malloc(sizeof(tcb));
    temp->threadContext = (ucontext_t *) malloc(sizeof(ucontext_t));
    temp->threadID = 0; // Garbage value
    temp->threadPriority = 0; // Assume low priority to begin 
    temp->next = NULL; // Initially it is just a single node

    getcontext(temp->threadContext); // let temp be the context of main() 
    running = temp;
    ready = NULL;
}


/**
 * t_create, creates a thread with the starting function consuming an int. The new thread also has an ID and a 
 *           given priority.
 * 
 * @param func -> void(*func)(int), the start funuction
 * @param id -> int, the id of the thread
 * @param pri -> int, the priority of the thread
 * @return None
 */
int t_create(void (*func)(int), int id, int pri) {
    size_t sz = 0x10000;
    ucontext_t *uc = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(uc); // Gets the context for the thread

    // Given, stores info for stackpointer, stack size, stack flags...
    uc->uc_stack.ss_sp = malloc(sz);
    uc->uc_stack.ss_size = sz;
    uc->uc_stack.ss_flags = 0;
    uc->uc_link = running->threadContext; 
    makecontext(uc, (void (*)(void)) func, 1, id);
    
    // Standard linked list push operation
    struct tcb **tracker = &ready, *customBlock;
    while(*tracker) // Get to last node, no need to worry about edge case
        tracker = &(*tracker)->next;
    // New thread control, block for the new thread
    customBlock = (struct tcb *) malloc(sizeof(tcb));
    customBlock->threadContext = uc;
    customBlock->threadID = id;
    customBlock->threadPriority = pri;
    customBlock->next = *tracker; // NULL 
    *tracker = customBlock;
}


/**
 * t_shutdown, shuts down this thread library by freeing all dynamically allocated memory.
 * 
 * @param None
 * @return None
 */
void t_shutdown() {
    struct tcb *temp = ready, *temp2;
    while(temp) { // Free the ready queue
      temp2 = temp;
      temp = temp->next;
      free(temp2->threadContext->uc_stack.ss_sp);
      free(temp2->threadContext);
      free(temp2);
    } // Free the one running
    free(running->threadContext->uc_stack.ss_sp);
    free(running->threadContext);
    free(running);
}


/**
 * t_terminate, terminates the calling thread by removing and freeing the TCB from the running queue. The thread 
 *              at the head of the ready queue is then resumes running using setcontext.
 * 
 * @param None
 * @return None
 */
void t_terminate() {
    free(running->threadContext->uc_stack.ss_sp);
    free(running->threadContext);
    free(running);
    running = ready; // Head
    ready = ready->next;
    running->next = NULL; // One running thread
    setcontext(running->threadContext);
}
