#ifndef WINDOWH
#define WINDOWH
#include <ncurses.h>

//Basic window operations


WINDOW *create_newwin(int height, int width, int starty, int startx);

void erasewindow(WINDOW* window);

void wprintctr(WINDOW* window, unsigned int height, unsigned int width, char* string);
void wprintctrx(WINDOW* window, int y, unsigned int width, char* string);
void printProgress(WINDOW* win, int current, int max);

#endif
