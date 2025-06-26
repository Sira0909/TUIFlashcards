#include "window.h"
WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);

	return local_win;
}
void erasewindow(WINDOW* window){
    wbkgd(window, COLOR_PAIR(1));
    werase(window);
    wrefresh(window);
    delwin(window);
}
