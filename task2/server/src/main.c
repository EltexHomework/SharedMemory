#include "../headers/server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

struct server* server;

void handle_signal(int signal);
void free_memory();

int main(void) {
  signal(SIGINT, handle_signal);
  server = create_server();
  atexit(free_memory);
  run_server(server);
  exit(EXIT_SUCCESS);
}

void handle_signal(int signal) {
  if (signal == SIGINT) {
    printf("Caught SIGINT, cleaning up...\n");
    free_memory();
    exit(EXIT_SUCCESS);
  }
}

void free_memory() {
  if (server != NULL) {
    free_server(server);
    server = NULL;
  }
}
