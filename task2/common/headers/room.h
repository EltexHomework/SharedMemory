#ifndef ROOM_H
#define ROOM_H

#include <stdlib.h>
#include "config.h"
#include "user.h"
#include "message.h"

/*
 * Used to define chat room with users and
 * messages.
 */
struct room {
  struct user users[MAX_USERS];
  struct message messages[MAX_CHAT_MESSAGES];
  int users_number;
  int messages_number;
};

struct room* create_room();

int add_room_user(struct room* room, struct user user);

int add_room_message(struct room* room, struct message message);

void delete_room_message(struct room* room, int id);

void delete_room_user(struct room* room, int id);

void free_room(struct room* room);

#endif // !ROOM_H
