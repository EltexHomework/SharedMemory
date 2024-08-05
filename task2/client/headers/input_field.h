#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H
#define STR_SIZE 128 

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Input field struct implements functionality
 * of text field.
 * 
 * [Key bindings]
 * 
 * - Enter - used to send message
 * - Tab - used to change focus on windows
 * - Key down - used on all windows to scroll down
 * - Key up - used on all windows to scroll up
 * - F1 - used to exit program
 * 
 * [Example]
 *  
 */
struct input_field {
  WINDOW* window;
  char str[STR_SIZE];
  int offset;

  int height;
  int width;
};

struct input_field* create_field(int width, int height, int start_y, int start_x);

void move_field_down(struct input_field* field);

void move_field_up(struct input_field* field);

void update_field(struct input_field* field);

int process_field_input(struct input_field* field);

void add_char(struct input_field* field, char c); 

char* get_str(struct input_field* field);

void clear_str(struct input_field* field);

void dispose_field(struct input_field* field);

#endif // !INPUT_FIELD_H

