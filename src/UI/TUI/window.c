#include <ncurses.h>
#include <windows/window.h>
#include <string.h>
#include <macros.h>
void printProgress(WINDOW* win, int current, int max){
    wmove(win, 0, 1); 
    int x, __attribute__((unused)) y;
    getmaxyx(win, y, x);
    int width;
    char testbuffer[20]; sprintf(testbuffer, "%d/%d", current+1, max); width = strlen(testbuffer);   // get the width of the progress bar to right align
    mvwaddch(win, 0, x-4-width, ACS_HLINE);
    mvwaddch(win, 0, x-3-width, ACS_RTEE);
    wprintw(win, "%d/%d", current+1, max);
    waddch(win, ACS_LTEE);
}
void printHeader(WINDOW* win, char* string){
    mvwaddch(win, 0, 2, ACS_HLINE);
    waddch(win, ACS_RTEE);
    wprintw(win, "%s", string);
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

void wprintctr(WINDOW* window, unsigned int height, unsigned int width, char* string){
    mvwprintw(window, (height-stringlen(string)/width)/2, (width > stringlen(string))?(width-stringlen(string))/2 : 0, "%s", string);
}
void wprintctrx(WINDOW* window, int y, unsigned int width, char* string){
    mvwprintw(window, y, (width-stringlen(string))/2, "%s", string);
}


