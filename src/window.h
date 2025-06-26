#ifndef WINDOWH
#define WINDOWH
#include <ncurses.h>
#include "helpers.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void erasewindow(WINDOW* window);
#endif
