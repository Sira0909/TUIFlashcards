#ifndef WINDOWH
#define WINDOWH
#include <ncurses.h>

//Basic window operations


//creates a window with given values
WINDOW *create_newwin(int height, int width, int starty, int startx);


//erases and deletes a window
void erasewindow(WINDOW* window);
#endif
