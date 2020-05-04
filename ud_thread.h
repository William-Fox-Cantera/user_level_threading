#include "t_lib.h"

/* 
 * thread library function prototypes
 */
void t_create(void(*function)(int), int thread_id, int priority);
void t_yield(void);
void t_init(void);
void t_terminate();
void t_shutdown();
void sem_destroy(sem_t **sp);
void sem_signal(sem_t *sp);
void sem_wait(sem_t *sp);
int sem_init(sem_t **sp, int sem_count);