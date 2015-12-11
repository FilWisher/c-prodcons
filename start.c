/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

#include "helper.h"

int main (int argc, char **argv)
{
  int queue_size, shm_id, sem_id;
  QUEUE *queue;
 
  /* initialize shared memory */
  key_t key = ftok(".", 'b');
  
  if (argc < 2) {
    printf("usage: start <queue-size>\n");
    exit(1);
  }
  
  /* read in max queue size */
  if ((queue_size = atoi(argv[1])) < 1) {
    printf("queue must be at least size of 1\n");
    exit(1);
  }
 
  /* initialize shared memory segment */
  if ((shm_id = shmget(key, sizeof(QUEUE) + 500 * (sizeof(JOBTYPE)), 
      0644 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  
  if ((queue = attach_to_queue(shm_id)) == (QUEUE *)-1) {
    exit(1);
  }
  queue->size = queue_size;
  queue->front = 0;
  queue->end = 0;
  
  /* create 4 semaphores */
  if ((sem_id = sem_create(key, 4)) == -1) {
    perror("sem_create"); 
    exit(1);
  } 
  if (sem_init(sem_id, MUTEX, 1) == -1) {
    perror("sem_init MUTEX");
    exit(1);
  }
  if (sem_init(sem_id, NOT_FULL, queue_size) == -1) {
    perror("sem_init NOT_FULL");
    exit(1);
  }
  if (sem_init(sem_id, NOT_EMPTY, 0) == -1) {
    perror("sem_init NOT_EMPTY");
    exit(1);
  }
  if (sem_init(sem_id, CONSUMERS, 0) == -1) {
    perror("sem_init CONSUMERS");
    exit(1);
  }
  
  return 0;
}
