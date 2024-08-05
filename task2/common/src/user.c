#include "../headers/user.h"

/**
 * create_user - used to create an object of user struct and
 * initialize it. Terminates username with \0 symbol.
 * @id - user identifier
 * @username - nickname of user
 *
 * Return: object of user struct
 */
struct user create_user(int id, char* username) {
  struct user user;

  user.id = id;
  strncpy(user.username, username, USERNAME_LEN - 1);
  user.username[USERNAME_LEN - 1] = '\0';

  return user;
}
