#include "t_lib.h"

/**
 * This is a user level library for making multi-threaded programs.
 * 
 * @author William Cantera --> (wcantera@udel.edu)
 * @version 0.0.1
 */


tcb *running; // Make thread control blocks for the ready and running queues
tcb *ready;


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
        tcb *currentlyRunning, *wantsToRun, **tracker = &ready;
        currentlyRunning = running;
        wantsToRun = ready;
        while(*tracker) tracker = &(*tracker)->next;
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
    tcb *temp = (tcb *) malloc(sizeof(tcb));
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
    tcb **tracker = &ready, *customBlock;
    while(*tracker) tracker = &(*tracker)->next;
    // New thread control, block for the new thread
    customBlock = (tcb *) malloc(sizeof(tcb));
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
    tcb *temp = ready, *temp2;
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

//______________________________________________________________________________________________________________________
// Semaphore functions


/**
 * sem_init, initializes a semaphore with a count, pointed to by stack pointer.
 * 
 * @param sp -> Stack pointer
 * @param sem_count -> the count it starts at
 * @return sem_count -> int, the count
 */
int sem_init(sem_t **sp, int sem_count) {
    *sp = malloc(sizeof(sem_t));
    (*sp)->count = sem_count;
    (*sp)->q = NULL; // No thread control block starting out
    return sem_count;
}


/**
 * sem_wait, current thread does a wait (p) on the specified semaphore.
 * 
 * @param sp -> sem_t, the semaphore to wait on
 * @return None
 */
void sem_wait(sem_t *sp) {
    sighold();
    sp->count--; // p operation
    if (sp->count < 0) {
		  tcb *currentlyRunning = running, **tracker;
      running = ready;
      ready = ready->next;
      tracker = &(sp->q);
      while (*tracker) tracker = &(*tracker)->next;
      *tracker = currentlyRunning; // Put it at the end of the sem_t list
      running->next = NULL; // Cut away from ready
		  swapcontext(currentlyRunning->threadContext, running->threadContext);
      sigrelse();
    } else {
      sigrelse();
    }
}


/**
 * sem_signal, the thread that signals becomes ready, and the first waiting thread, if there is any, becomes ready.
 * 
 * @param sp -> sem_t, the semaphore to signal
 * @return None
 */
void sem_signal(sem_t *sp) {
    sighold();
    sp->count++; // v operation
    if (sp->count <= 0) { // If no tcb exists yet, don't run
        tcb **tracker = &ready, *semTemp = sp->q;
        sp->q = sp->q->next; // Remove it
        semTemp->next = NULL;
        while (*tracker) tracker = &(*tracker)->next; // Get to the last tcb
        *tracker = semTemp; // Push tcb to ready
        sigrelse();
    } else {
        sigrelse();
    }
  } 


/** 
 * sem_destroy, frees all memory allocated to the given semaphore.
 * 
 * @param sp -> sem_t, the semaphore to be freed
 * @return None
 */
void sem_destroy(sem_t **sp) {
    free(*sp); // That's it
}
