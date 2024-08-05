#ifndef CHAT_H
#define CHAT_H

#include "chat_window.h"
#include "input_field.h"
#include "users_window.h"
#include "../../common/headers/config.h"
#include "../../common/headers/room.h"
#include "../../common/headers/user.h"
#include "../../common/headers/message.h"
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Chat struct implements chat logic and connection
 * to server.
 * 
 * [Key bindings]
 * 
 * - F1 - used to exit program
 * 
 * [Example]
 * initscr();
 * cbreak();
 * noecho();
 * keypad(stdscr, TRUE);
 *
 * char username[USERNAME_LEN];
 *
 * get_username(username, USERNAME_LEN);
 *
 * int width, height;
 * getmaxyx(stdscr, height, width);
 *
 * struct chat* chat = create_chat(username, height, width, 0, 0);
 * run_chat(chat);
 *
 * endwin();
 */
struct chat {
  struct chat_window* chat_window;
  struct users_window* users_window;
  struct input_field* input_field;
  struct room* room;
  struct sembuf lock[2];
  struct sembuf unlock;
  
  int id;
  char username[USERNAME_LEN];

    
  /* specifies which window has focus on:
   * 1 - chat_window,
   * 2 - users_window,
   * 3 - input_field
   */
  int current_window;  
  
  /* Shared memory segment identifier */
  int shm_id;
  /* Semaphore identifier */
  int sem_id;
};

struct chat* create_chat(char* username, int height, int width, int start_y, int start_x);

void run_chat(struct chat* chat);

void connect_to_server(struct chat* chat);

void disconnect_from_server(struct chat* chat);

void* handle_room_updates(void* args);

void update_users(struct chat* chat);

void delete_users(struct chat* chat);

void update_messages(struct chat* chat);

void send_message(struct chat* chat);

int generate_user_id(struct chat* chat);

int generate_message_id(struct chat* chat);

int open_sem(char* filename, int id);

int open_shm(char* filename, int id);
 
void dispose_chat(struct chat* chat);

#endif // !CHAT_H
