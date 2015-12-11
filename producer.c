/*
    PRODUCER
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "helper.h"

int main (int argc, char *argv[])
{
  int id, job_count, shm_id, duration, start, sem_id;
  QUEUE *queue;

  /* seed values for random number */
  srand(time(NULL)); 

  if (argc < 3) {
    printf("usage: producer <id> <job_count>\n");
    return -1;
  }
  
  /* read id of producer and num jobs to create*/
  id = atoi(argv[1]);
  job_count = atoi(argv[2]);

  /* key is same for programs in same directory */
  key_t key = ftok(".", 'b');
  
  if ((shm_id = get_shared_memory(key)) == -1) {
    perror("get_shared_memory");
    exit(1); 
  }
 
  /* try to associate with shared memory with queue pointer */ 
  if ((queue = attach_to_queue(shm_id)) == (QUEUE *)-1) {
    perror("attach_to_queue");
    exit(1);    
  }

  /* attach to semaphore */
  if ((sem_id = sem_attach(key)) == -1) {
    perror("semid");
    exit(1);
  }
 
  /* record time of start */
  start = (int)time(NULL);
  for (int i = 0; i < job_count; i += 1) { 

    /* random time to wait (2-7 seconds) */
    duration = (rand() % 6) + 2;
    
    sem_wait(sem_id, NOT_FULL);
    sem_wait(sem_id, MUTEX);
  
    /* CRITICAL SECTION */
    printf("Producer(%d) time %d: Job id %d duration %d\n", 
        id, elapsed_time(start), queue->end + 1, duration);
    queue->job[queue->end].id = queue->end + 1;
    queue->job[queue->end].duration = duration;
    queue->end = (queue->end + 1) % queue->size;
    /* END OF CRITICAL SECTION */

    sem_signal(sem_id, MUTEX);
    sem_signal(sem_id, NOT_EMPTY);
    
    /* sleep between 2 and 4 seconds */ 
    sleep((rand() % 3) + 2);
  }

  printf("Producer(%d) time %d: No more jobs to generate.\n", 
      id, elapsed_time(start));
 
  /* wait until NO consumers left active before exiting */
  sem_wait_zero(sem_id, CONSUMERS);
  /* close all semaphores */  
  sem_close(sem_id);
  /* quit when all jobs have been produced */
  detach(shm_id, queue);
  
  return 0;
}
