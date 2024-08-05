#ifndef CONFIG_H
#define CONFIG_H

#define MAX_USERS 32
#define MAX_CHAT_MESSAGES 128
#define SERVER_FILE "./server"
#define SHM_ID 1
#define SEM_ID 2 
#define USERNAME_LEN 64
#define MESSAGE_LEN 128
#define print_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0) 

#endif // !CONFIG_H

