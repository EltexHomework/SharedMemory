#include "../headers/server.h"
#include <stdio.h>

/**
 * create_server - used to create server object. Mallocs
 * memory for collections, opens queues, inits mutexes.
 * 
 * Return: pointer to object of server struct 
 */
struct server* create_server() {
  struct server* server = (struct server*) malloc(sizeof(struct server));

  /* Initialize server */  
  server->shm_id = open_shm(SERVER_FILE, SHM_ID);
  server->sem_id = open_sem(SERVER_FILE, SEM_ID);
  
  server->room = (struct room*) shmat(server->shm_id, NULL, 0);

  return server;
}

/**
 * run_server - used to run previously created server,
 * creates thread for new_messages_queue, calls
 * connection requests handler.
 * @server - pointer to an object of server struct 
 */
void run_server(struct server* server) {
  fprintf(stderr, "Server started.\n");   
  handle_room_updates(server);
}

/**
 * handle_room_updates - used to log changes in room.
 * @server - pointer to an object of server struct
 */
void handle_room_updates(struct server* server) {
  struct sembuf lock[2] = {{0, 0, 0}, {0, 1, 0}};
  struct sembuf unlock = {0, -1, 0};
  
  int last_message_index = server->room->messages_number - 1;

  /* Get last message and user ids */
  int prev_msgs_id = 0; 
  int prev_user_number = server->room->users_number;

  /* Check for changes in room */
  while (1) {
    /* Lock semaphore */
    if (semop(server->sem_id, lock, 2) == -1)
      print_error("semop lock");

    //fprintf(stderr, "Server is running\n");
    last_message_index = server->room->messages_number - 1;
        
    /* Users changed */
    if (server->room->users_number < prev_user_number) {
      fprintf(stderr, "User disconnected\n");
      prev_user_number = server->room->users_number;
    }
    else if (server->room->users_number > prev_user_number) {
      fprintf(stderr, "New user connected\n");
      prev_user_number = server->room->users_number;
    }
    
    if (last_message_index != -1) {
      /* Messages changed */
      if (server->room->messages[last_message_index].id != prev_msgs_id) {
        char *username = server->room->messages[last_message_index].username;
        char *message = server->room->messages[last_message_index].text;
        
        fprintf(stderr, "New message: %s %s\n", username, message);
        prev_msgs_id = server->room->messages[last_message_index].id; 
      }
    }
  
    /* Unlock semaphore */
    if (semop(server->sem_id, &unlock, 1) == -1)
      print_error("semop unlock");
  }
}

/**
 * open_sem - used to open semaphore.
 * @filename - name of file that used in ftok
 * @id - identifier that used in ftok
 *
 * Return: semaphore identifier if successful 
 */
int open_sem(char* filename, int id) {
  key_t key;
  int sem_id;
  
  /* Get sem key */
  key = ftok(filename, id);
  if (key == -1)
    print_error("ftok");
  
  /* Get semaphore id */
  sem_id = semget(key, 1, IPC_CREAT | 0666);
  if (sem_id == -1)
    print_error("semget");
  
  return sem_id;
}

/**
 * open_shm - used to open shared memory segment.
 * @filename - name of file that used in ftok
 * @id - identifier that used in ftok
 *
 * Return: shared memory segment identifier if successful 
 */
int open_shm(char* filename, int id) {
  key_t key;
  int shm_id;

  key = ftok(filename, id);
  if (key == -1)
    print_error("ftok");

  shm_id = shmget(key, sizeof(struct room), IPC_CREAT | 0666);
  if (shm_id == -1)
    print_error("shmget");

  return shm_id;
}

/**
 * free_server - used to free memory allocated for server.
 * Frees collection, closes queues, destroys mutexes. 
 * @server - pointer to an object of server struct.
 */ 
void free_server(struct server* server) {
  /* Delete shared memory segment */
  if (shmctl(server->shm_id, IPC_RMID, NULL) == -1)
    print_error("shmctl error");

  /* Delete semaphore */
  if (semctl(server->sem_id, IPC_RMID, 0, 0, 0) == -1)
    print_error("semctl error");

  free(server);
}
