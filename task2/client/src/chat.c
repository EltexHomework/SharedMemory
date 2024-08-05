#include "../headers/chat.h"
#include <pthread.h>
#include <sys/shm.h>

/**
 * create_chat - creates chat struct object, calculated width
 * and height of windows, opens queues.
 * @username: nickname of user
 * @height: height of chat window
 * @width: width of chat window
 * @start_y: y position from where chat window is drawn
 * @start_x: x position from where chat window is drawn
 *
 * Return: pointer to struct chat if successful, or NULL as error code. 
 */ 
struct chat* create_chat(char* username, int height, int width, int start_y, int start_x) {
  struct chat* chat = (struct chat*) malloc(sizeof(struct chat));
  int chat_window_height = height - 4; 
  int chat_window_width = width - 30; 

  int users_window_height = height - 4; 
  int users_window_width = 30; 

  int input_field_height = 4; 
  int input_field_width = width; 
  
  /* Create ui */
  chat->chat_window = create_chat_window(chat_window_height, chat_window_width, start_y, start_x);
  chat->users_window = create_users_window(users_window_height, users_window_width, start_y, start_x + chat_window_width);
  chat->input_field = create_field(input_field_height, input_field_width, start_y + chat_window_height, start_x);
  chat->current_window = 1;
  
  /* Open shm segment and semaphore */
  chat->shm_id = open_shm(SERVER_FILE, SHM_ID);
  chat->sem_id = open_sem(SERVER_FILE, SEM_ID);
  
  /* Initialize semaphore operations */
  chat->lock[0].sem_num = 0;
  chat->lock[0].sem_op = 0;
  chat->lock[0].sem_flg = 0;
  chat->lock[1].sem_num = 0;
  chat->lock[1].sem_op = 1;
  chat->lock[1].sem_flg = 0;
  chat->unlock.sem_num = 0;
  chat->unlock.sem_op = -1;
  chat->unlock.sem_flg = 0;

  strncpy(chat->username, username, USERNAME_LEN - 1);
  chat->username[USERNAME_LEN - 1] = '\0';

  update(chat->users_window->listbox);
  update(chat->chat_window->listbox);
  
  return chat;
}
/**
 * run_chat - used to establish connection to server
 * and run chat functionality.
 * @chat - pointer to chat struct object
 */
void run_chat(struct chat* chat) {
  pthread_t room_thread;
  
  connect_to_server(chat);
  
  if (pthread_create(&room_thread, NULL, handle_room_updates, chat) != 0) {
    perror("pthread_create error");
    exit(EXIT_FAILURE);
  }

  while (1) {
    /* Windows focus on chat_window */
    if (chat->current_window == 1) {
      if (process_chat_window_input(chat->chat_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    }
    /* Windows focus on users_window */
    else if (chat->current_window == 2) {
      if (process_users_window_input(chat->users_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    } 
    /* Windows focus on input_field */
    else {
      int result;
      if ((result = process_field_input(chat->input_field)) == 1) {
        chat->current_window = 1;
      }
      /* Received Enter key */
      else if (result == 2) {
        send_message(chat);         
      } 
      else {
        break;
      }
    }
  }
  
  pthread_cancel(room_thread);
  /* Unlock semaphore */
  if (semop(chat->sem_id, &chat->unlock, 1) == -1)
    print_error("semop unlock");
 
  disconnect_from_server(chat);  
  endwin();
}

/**
 * connect_to_server - used to establish connection
 * to server, which config specified in config.h.
 * connection_msg struct is used as a message for message
 * queue.
 * @chat - pointer to chat struct object
 */
void connect_to_server(struct chat* chat) {
  /* Lock semaphore */
  if (semop(chat->sem_id, chat->lock, 2) == -1)
    print_error("semop lock");
   
  chat->room = (struct room*) shmat(chat->shm_id, NULL, SHM_RND);
  if (chat->room == (void *) -1)
    print_error("shmat");
  
  chat->id = generate_user_id(chat);   
    
  struct user user = create_user(chat->id, chat->username);
  if (add_room_user(chat->room, user) == -1)
    print_error("add_room_user: full room");
   
  update_users(chat);
  update_messages(chat);
  /* Unlock semaphore */
  if (semop(chat->sem_id, &chat->unlock, 1) == -1)
    print_error("semop unlock");
}

/**
 * disconnect_from_server - used to send disconnect message
 * to server.
 * @chat - pointer to chat struct object
 */
void disconnect_from_server(struct chat* chat) {
  /* Lock semaphore */
  if (semop(chat->sem_id, chat->lock, 2) == -1)
    print_error("semop lock");
    
  delete_room_user(chat->room, chat->id);

  /* Unlock semaphore */
  if (semop(chat->sem_id, &chat->unlock, 1) == -1)
    print_error("semop unlock");
}

/**
 * handle_room_updates - used to check chat room for updates of
 * messages and users. Locks semaphore, checks for updates, and
 * updates ui.
 * @args - pointer to chat struct, must be cast manually
 */
void* handle_room_updates(void* args) {
  struct chat* chat = (struct chat*) args;
  int last_message_index = chat->room->messages_number - 1;

  /* Get last message and user ids */
  int prev_msgs_id = chat->room->messages[last_message_index].id;
  int prev_user_number = chat->room->users_number;

  while (1) {
    /* Lock semaphore */
    if (semop(chat->sem_id, chat->lock, 2) == -1)
      print_error("semop lock");
    
    last_message_index = chat->room->messages_number - 1;

    /* Users changed */
    if (chat->room->users_number != prev_user_number) {
      delete_users(chat);
      update_users(chat);
      prev_user_number = chat->room->users_number;
    }
     
    /* Messages changed */
    if (last_message_index != -1) {
      if (chat->room->messages[last_message_index].id != prev_msgs_id) {
        char *username = chat->room->messages[last_message_index].username;
        char *message = chat->room->messages[last_message_index].text;

        add_message(chat->chat_window, username, message);
        update(chat->chat_window->listbox);
        prev_msgs_id = chat->room->messages[last_message_index].id; 
      }
    }

    /* Unlock semaphore */
    if (semop(chat->sem_id, &chat->unlock, 1) == -1)
      print_error("semop unlock");
  }
  return NULL;
}

/**
 * update_users - used to update users_window when
 * room.users is changed.
 * @chat - pointer to an object of chat struct 
 */
void update_users(struct chat* chat) {
  for (int i = 0; i < chat->room->users_number; i++) {
    add_user(chat->users_window, chat->room->users[i].username);
  }
  update(chat->users_window->listbox);
}

/**
 * delete_users - used to clear users_window when
 * room.users us changed.
 * @chat - pointer to an object of chat struct 
 */
void delete_users(struct chat* chat) {
  clear_items(chat->users_window->listbox); 
  update(chat->users_window->listbox);
}

/**
 * update_messages - used to update chat_window when
 * room.messages is changed.
 * @chat - pointer to an object of chat struct 
 */
void update_messages(struct chat* chat) {
  for (int i = 0; i < chat->room->messages_number; i++) {
    add_message(chat->chat_window, chat->room->messages[i].username, chat->room->messages[i].text);
  }
  update(chat->chat_window->listbox);
}

/**
 * send_message - used to send message to shared
 * memory segment.
 * @chat - pointer to chat struct object
 */
void send_message(struct chat* chat) {
  /* Lock semaphore */
  if (semop(chat->sem_id, chat->lock, 2) == -1)
    print_error("semop lock");

  char* text = get_str(chat->input_field);
  int id = generate_message_id(chat);
  struct message message = create_message(id, chat->username, text);
  
  /* Send new message */ 
  if (add_room_message(chat->room, message) == -1) {
    /* Messages are full, delete latest one */
    delete_room_message(chat->room, chat->room->messages[0].id);
    /* Add message to the end */
    add_room_message(chat->room, message);
  }
  
  /* Unlock semaphore */
  if (semop(chat->sem_id, &chat->unlock, 1) == -1)
    print_error("semop unlock");
}

/**
 * generate_user_id - used to generate user id
 * based on last user in room.
 */
int generate_user_id(struct chat* chat) {
  /* Users are empty */
  if (chat->room->users_number == 0)
    return 1;

  int index = chat->room->users_number - 1;
  return chat->room->users[index].id + 1;
}

/**
 * generate_message_id - used to generate message
 * based on last message in room
 */
int generate_message_id(struct chat* chat) {
  /* Messages are empty */
  if (chat->room->messages_number == 0)
    return 1;

  int index = chat->room->messages_number - 1;
  return chat->room->messages[index].id + 1;
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
  sem_id = semget(key, 1, 0666);
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

  shm_id = shmget(key, sizeof(struct room), 0666);
  if (shm_id == -1)
    print_error("shmget");

  return shm_id;
}

/**
 * dispose_chat - used to free memory allocated for chat.
 * Should be called in onexit function to free memory on SIGINT
 * signal.
 * @chat - pointer to chat struct object
 */
void dispose_chat(struct chat* chat) {
  dispose_chat_window(chat->chat_window);
  dispose_users_window(chat->users_window);
  dispose_field(chat->input_field);
  
  if (shmdt(chat->room) == -1)
    print_error("shmdt");
  
  free(chat);
}

