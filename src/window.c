#include <ncurses.h>
#include <windows/window.h>
#include <string.h>
void printProgress(WINDOW* win, int current, int max){
    wmove(win, 0, 1); 
    int x, __attribute__((unused)) y;
    getmaxyx(win, y, x);
    int width;
    char testbuffer[20]; sprintf(testbuffer, "%d/%d", current+1, max); width = strlen(testbuffer);   // get the width of the progress bar to right align
    mvwaddch(win, 0, x-3-width, ACS_RTEE);
    wprintw(win, "%d/%d", current+1, max);
    waddch(win, ACS_LTEE);
}
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
    mvwprintw(window, (height-strlen(string)/width)/2, (width > strlen(string))?(width-strlen(string))/2 : 1, "%s", string);
}
void wprintctrx(WINDOW* window, int y, int width, char* string){
    mvwprintw(window, y, (width-strlen(string))/2, "%s", string);
}


