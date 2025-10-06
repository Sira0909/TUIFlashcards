#ifndef WINDOWH
#define WINDOWH
#include <ncurses.h>

//Basic window operations


WINDOW *create_newwin(int height, int width, int starty, int startx);

void erasewindow(WINDOW* window);

void wprintctr(WINDOW* window, int height, int width, char* string);
void wprintctrx(WINDOW* window, int y, int width, char* string);

#endif
