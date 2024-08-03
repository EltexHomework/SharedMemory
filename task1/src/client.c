#include "../header/config.h"

#define run_client() do {if (STANDARD) {run_posix();} else {run_sysv();}} while(0)
#define print_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)

void run_posix();

void run_sysv();

int main(void) {
  run_client();
  exit(EXIT_SUCCESS);
}

/**
 * run_posix - used to run server with shared memory segment
 * on POSIX standard.
 */
void run_posix() {
  int fd, sem_id;
  char* message = "Hello!";
  struct shmbuf* shm;
  
  /* Create shm segment */
  fd = shm_open(SHM_POSIX_FILE, O_RDWR, 0666);
  if (fd == -1)
    print_error("shm_open");
  
  /* Map shared memory segment to user space */ 
  shm = mmap(NULL, sizeof(struct shmbuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED)
    print_error("mmap");
  
  /* Read message from segment */  
  if (sem_wait(&shm->sem1) == -1)
    print_error("sem_wait");

  printf("Message received: %s\n", shm->buf);

  /* Write message to segment */
  shm->cnt = strlen(message);
  strncpy(shm->buf, message, strlen(message));
  
  /* Send message to client */     
  if (sem_post(&shm->sem2) == -1)
    print_error("sem_post");
}

/**
 * run_sysv - used to run server with shared memory segment
 * on System V standard.
 */
void run_sysv() {
  struct sembuf lock[2] = {{0, 0, 0}, {0, 1, 0}};
  struct sembuf unlock = {0, -1, 0};

  key_t shm_key, sem_key;
  int shm_id, sem_id;
  char* buffer;
  char* message = "Hello!";
  
  shm_key = ftok(SHM_FILE, SHM_ID);
  if (shm_key == -1)
    print_error("ftok error");
  
  sem_key = ftok(SHM_FILE, SEM_ID);
  if (sem_key == -1)
    print_error("ftok error");

  /* Create shared memory segment */
  shm_id = shmget(shm_key, MESSAGE_LEN, 0666);
  if (shm_id == -1)
    print_error("shmget error");
  
  /* Create semaphore */ 
  sem_id = semget(sem_key, 1, 0666);
  if (sem_id == -1)
    print_error("semget error");
  
  /* Attach to shm segment */
  buffer = (char *) shmat(shm_id, NULL, SHM_RND);
  printf("Received message: %s\n", buffer);
  
  /* Write response */
  if (semop(sem_id, lock, 2) == -1)
    print_error("semop error");

  strncpy(buffer, message, strlen(message));
  printf("Sent response\n");
}


