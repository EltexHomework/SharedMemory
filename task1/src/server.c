#include "../header/config.h"
#include <fcntl.h>
#include <sys/mman.h>

#define run_server() do {if (STANDARD) {run_posix();} else {run_sysv();}} while(0)
#define print_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)

void run_posix();

void run_sysv();

int main(void) {
  run_server();
  exit(EXIT_SUCCESS);
}

/**
 * run_posix - used to run server with shared memory segment
 * on POSIX standard.
 */
void run_posix() {
  int fd;
  char* message = "Hi!";
  struct shmbuf* shm;
  
  /* Create shm segment */
  fd = shm_open(SHM_POSIX_FILE, O_CREAT | O_RDWR, 0666);
  if (fd == -1)
    print_error("shm_open");
  
  /* Allocate enough size for struct shmbuf */
  if (ftruncate(fd, sizeof(struct shmbuf)) == -1)
    print_error("ftruncate");
  
  /* Map shared memory segment to user space */ 
  shm = mmap(NULL, sizeof(struct shmbuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED)
    print_error("mmap");
  
  /* Initialize semaphores */
  if (sem_init(&shm->sem1, 1, 0) == -1)
    print_error("sem_init");
  if (sem_init(&shm->sem2, 1, 0) == -1)
    print_error("sem_init");

  /* Write message to segment */
  shm->cnt = (size_t) strlen(message);
  strncpy(shm->buf, message, MESSAGE_LEN);
  
  /* Send message to client */     
  if (sem_post(&shm->sem1) == -1)
    print_error("sem_post");
  
  /* Read response from client */
  if (sem_wait(&shm->sem2) == -1)
    print_error("sem_wait");

  printf("Received message: %s\n", shm->buf);

  shm_unlink(SHM_POSIX_FILE); 
}

/**
 * run_sysv - used to run server with shared memory segment
 * on System V standard
 */
void run_sysv() {
  struct sembuf lock[2] = {{0, 0, 0}, {0, 1, 0}};
  struct sembuf unlock = {0, -1, 0};

  key_t shm_key, sem_key;
  int shm_id, sem_id;
  char* buffer;
  char* message = "Hi!";

  shm_key = ftok(SHM_FILE, SHM_ID);
  if (shm_key == -1)
    print_error("ftok error");
  
  sem_key = ftok(SHM_FILE, SEM_ID);
  if (sem_key == -1)
    print_error("ftok error");

  /* Create shared memory segment */
  shm_id = shmget(shm_key, MESSAGE_LEN, IPC_CREAT | 0666);
  if (shm_id == -1)
    print_error("shmget error");
  
  /* Create semaphore */ 
  sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
  if (sem_id == -1)
    print_error("semget error");
  
  /* Attach to shm segment */
  buffer = (char*) shmat(shm_id, NULL, 0);
  if (buffer == (void*) -1)
    print_error("shmat error");
  
  /* Write message to shm */
  if (semop(sem_id, lock, 2) == -1)
    print_error("semop error");
  
  strncpy(buffer, message, strlen(message)); 

  if (semop(sem_id, &unlock, 1) == -1)
    print_error("semop error");
  
  /* Wait for client response */
  printf("Waiting for client response\n");

  if (semop(sem_id, &unlock, 1) == -1)
    print_error("semop error");

  printf("Received message: %s\n", buffer);
  
  /* Delete shared memory segment */
  if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    print_error("shmctl error");

  /* Delete semaphore */
  if (semctl(sem_id, IPC_RMID, 0, 0, 0) == -1)
    print_error("semctl error");
}

