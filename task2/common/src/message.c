#include "../headers/message.h"

/**
 * create_message - used to create and initialize an
 * object of message struct.
 * @id - message indentifier
 * @username - username of sender
 * @text - text of the message
 *
 * Return: an object of message struct
 */
struct message create_message(int id, char* username, char* text) {
  struct message message;

  message.id = id;
  strncpy(message.username, username, USERNAME_LEN);
  strncpy(message.text, text, MESSAGE_LEN);

  return message;
}
