#ifndef USER_H
#define USER_H

#include "config.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
 * User struct specifies connected user,
 * contains username of length USERNAME_LEN,
 * specified in config.h, and pid of user, used
 * to send messags directly to user.
 */
struct user {
  int id;
  char username[USERNAME_LEN];
};

struct user create_user(int id, char* username);

#endif // !USER_H
