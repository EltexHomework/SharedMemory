#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <curses.h>
#include <stdio.h>
#include "listbox.h"

/* Chat windows struct adds chat window logic to
 * blank listbox.
 * [Key bindings]
 * 
 * - Tab - used to change focus on windows
 * - Key down - used on all windows to scroll down
 * - Key up - used on all windows to scroll up
 * - F1 - used to exit program
 * 
 * [Example]
 *  
 */
struct chat_window {
  WINDOW* window;
  struct listbox* listbox;
};

struct chat_window* create_chat_window(int width, int height, int start_y, int start_x);

int process_chat_window_input(struct chat_window* chat_window);

void draw_border(struct chat_window* chat_window);

void add_message(struct chat_window* chat_window, char* sender, char* message);

void move_chat_up(struct chat_window* char_window);

void move_chat_down(struct chat_window* char_window);

void dispose_chat_window(struct chat_window* window);

#endif // !CHAT_WINDOW_H
