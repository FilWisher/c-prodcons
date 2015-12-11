/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_attach - Attach semaphore array to the program
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_wait_zero - Waits until specified semaphore is 0
 * sem_timewait - Waits on a semaphore for a particular time
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

#include "helper.h"
#include <time.h>

void detach(int id, void *loc) 
{
  shmdt(loc);
  shmctl(id, IPC_RMID, NULL);
}

QUEUE *attach_to_queue(int shm_id)
{
  QUEUE *queue = (QUEUE *)shmat(shm_id, (void *)0, 0); 
  if (queue == (QUEUE *)-1){
    perror("shmat");
    detach(shm_id, queue);
  }
  return queue;
}

int check_arg (char *buffer)
{
  int i, num = 0;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    if ((0+buffer[i]) > 57)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_attach (key_t key)
{
  int id;
  if ((id = semget (key, 1,  0)) < 0)
    return -1;
  return id;
}
 
int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0) {
    perror("semget");   
    return -1;
  }
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0) {
    perror("semctl");
    return -1;
  }
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, 0}
  };
  semop (id, op, 1);
}

void sem_wait_zero (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 0, 0}
  };
  semop (id, op, 1);
}

int sem_timewait (int id, short unsigned int num, int tim)
{
  struct timespec ts = {tim, 0};
  struct sembuf op[] = {
    {num, -1, 0}
  };
  if (semtimedop(id, op, 1, &ts ) == -1 )
    if (errno == EAGAIN)
      return -1;
  return 0;
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, 0}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}

int elapsed_time(int start)
{
  int time_now = (int)time(NULL); 
  int diff = time_now - start;
  return diff;
}

int get_shared_memory(int key)
{
  int shm_id = shmget(key, sizeof(QUEUE) + 500 * (sizeof(JOBTYPE)), 0644);
  if (shm_id == -1) {
    perror("shmget"); 
  }
  return shm_id;
}
