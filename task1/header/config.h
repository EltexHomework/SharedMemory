#ifndef CONFIG_H
#define CONFIG_H

#define SHM_FILE "./server"
#define SHM_POSIX_FILE "/shm"
#define SHM_ID 1
#define SEM_ID 2
#define MESSAGE_LEN 128

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

union semun {                   /* Used in calls to semctl() */
   int                 val;
   struct semid_ds *   buf;
   unsigned short *    array;
#if defined(__linux__)
   struct seminfo *    __buf;
#endif
};

/*
 * Used as a buffer to transfer through shared
 * memory.
 */
struct shmbuf {
  /* POSIX semaphores */
  sem_t sem1;
  sem_t sem2;

  /* Number of bytes used by buf */
  size_t cnt;
  /* Data being transfered */
  char buf[MESSAGE_LEN];
};

#endif
