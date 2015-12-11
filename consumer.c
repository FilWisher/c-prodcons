/*
    CONSUMER
*/

#include "helper.h"
#include <time.h>

int main (int argc, char *argv[])
{
  int id, shm_id, start, job_id, job_duration, sem_id;
  QUEUE *queue;
  
  if (argc < 2) {
    printf("usage: consumer <id>\n");
    return -1;
  }
  
  /* take id of consumer from commandline */
  id = atoi(argv[1]);
 
  /* key is same for programs in same directory */
  key_t key = ftok(".", 'b');
  
  /* associate with shared memory segment */
  if ((shm_id = get_shared_memory(key)) == -1) {
    perror("get_shared_memory");
    exit(1);
  }
 
  /* try to associate with shared memory with queue pointer */ 
  if ((queue = attach_to_queue(shm_id)) == (QUEUE *)-1) {
    perror("attach_to_queue");
    detach(shm_id, queue);
    exit(1);
  }
  
  /* associate with semaphore */
  if ((sem_id = sem_attach(key)) == -1) {
    perror("sem_attach");
    exit(1);
  }

  /* signal a consumer is in existence */
  sem_signal(sem_id, CONSUMERS);
  
  /* start time of consumer activity */  
  start = (int)time(NULL);
  while(1) {
  
    /* wait for until queue NOT_EMPTY for max 10 seconds */
    if (sem_timewait(sem_id, NOT_EMPTY, 10) == -1) {
      /* if 10 seconds has passed detach and decrement number of consumers */
      printf("Consumer(%d) time %d: No jobs left.\n", id, elapsed_time(start));
      detach(shm_id, queue);
      sem_wait(sem_id, CONSUMERS);
      exit(0);
    }
    sem_wait(sem_id, MUTEX);
  
    /* CRITICAL SECTION */
    job_id = queue->job[queue->front].id;
    job_duration = queue->job[queue->front].duration;
    queue->front = (queue->front + 1) % queue->size;
    /* END OF CRITICAL SECTION */
    
    sem_signal(sem_id, MUTEX);
    sem_signal(sem_id, NOT_FULL);

    /* sleep for duration of job */
    printf("Consumer(%d) time %d: Job id %d executing sleeping duration %d\n", 
        id, elapsed_time(start), job_id, job_duration);
    sleep(job_duration);
    printf("Consumer(%d) time %d: Job id %d completed\n", 
        id, elapsed_time(start), job_id);
  
  }
  
  /* should never be reached */
  return 0;
}
