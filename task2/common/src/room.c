#include "../headers/room.h"

/**
 * create_room - used to create and object of room
 * struct.
 *
 * Return: pointer to an object of room struct
 */
struct room* create_room() {
  struct room* room = (struct room*) malloc(sizeof(struct room)); 
  
  room->users_number = 0;
  room->messages_number = 0;

  return room;
}

/**
 * add_room_user - used to add user to users collection.
 * @room - pointer to an object of room struct
 * @user - object of user struct
 *
 * Return: 0 if successful, -1 if users is full
 */
int add_room_user(struct room* room, struct user user) {
  /* Users are full */
  if (room->users_number == MAX_USERS)
    return -1;
  
  room->users[room->users_number] = user;
  room->users_number++;
  return 0;
}

/**
 * add_room_message - used to add message to messages collection.
 * @room - pointer to an object of room struct
 * @message - object of message struct
 *
 * Return: 0 if successful, -1 if messages are full
 */
int add_room_message(struct room* room, struct message message) {
  /* Messages are full */
  if (room->messages_number == MAX_CHAT_MESSAGES)
    return -1;

  room->messages[room->messages_number] = message;      
  room->messages_number++;
  return 0;
}

/**
 * delete_room_message - used to delete message from messages
 * by identifier.
 * @room - pointer to an object of room struct
 * @id - message identifier
 */
void delete_room_message(struct room* room, int id) {
  int index;
  
  /* Find index of message */
  for (int i = 0; i < room->messages_number; i++) {
    if (room->messages[i].id == id) {
      index = i;
      break;
    }
  }
  
  /* Move messages from index + 1 to left */ 
  for (int i = index; i < room->messages_number - 1; i++) {
    room->messages[i] = room->messages[i + 1]; 
  }
  
  room->messages_number--;
}

/**
 * delete_room_user - used to delete user from users by
 * identifier.
 * @room - pointer to an object of room struct
 * @id - user identifier
 */
void delete_room_user(struct room* room, int id) {
  int index = -1;
  
  /* Find index of user */
  for (int i = 0; i < room->users_number; i++) {
    if (room->users[i].id == id) {
      index = i;
      break;
    }
  }
  
  if (index == -1)
    return;

  /* Move users from index + 1 to left */ 
  for (int i = index; i < room->users_number - 1; i++) {
    room->users[i] = room->users[i + 1]; 
  }

  /* Clear the last user slot */
  memset(&room->users[room->users_number - 1], 0, sizeof(struct user));
  room->users_number--;
}

/**
 * free_room - used to free allocated memory for room
 * object.
 * @room - pointer to an object of room struct 
 */
void free_room(struct room* room) {
  free(room);
}
