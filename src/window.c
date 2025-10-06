#include <ncurses.h>
#include <windows/window.h>
#include <string.h>
WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);

	return local_win;
}

//erases and deletes a window
void erasewindow(WINDOW* window){
    wbkgd(window, COLOR_PAIR(1));
    werase(window);
    wrefresh(window);
    delwin(window);
}

void wprintctr(WINDOW* window, int height, int width, char* string){
    mvwprintw(window, height/2, (width-strlen(string))/2, "%s", string);
}
void wprintctrx(WINDOW* window, int y, int width, char* string){
    mvwprintw(window, y, (width-strlen(string))/2, "%s", string);
}
