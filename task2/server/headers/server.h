#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include "../../common/headers/user.h"
#include "../../common/headers/message.h"
#include "../../common/headers/room.h"
#include "../../common/headers/config.h"

/*
 * Server struct implements functionality for
 * messanger server with one room, capable of
 * connecting and disconnecting users. Message
 * queue are used to transfer messages between
 * server and client.
 *
 * [Example]
 * struct server* server;
 *
 * void free_memory();
 *
 * int main(void) {
 *  server = create_server();
 *  run_server(server); 
 *  return 0;  
 * }
 * 
 * void free_memory() {
 *  free_server(server); 
 * }
 */
struct server {
  struct room* room;  
  
  int shm_id;
  int sem_id;
};

struct server* create_server();

void run_server(struct server* server);

void handle_room_updates(struct server* server);

int open_sem(char* filename, int id);

int open_shm(char* filename, int id);

void free_server(struct server* server);
#endif // !SERVER_H
