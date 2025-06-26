#include "WIN.h"

void init_win_params_simple(WIN *p_win, int height, int width, int color){
    p_win->height = height;
    p_win->width = width;
    p_win->starty = (LINES - p_win->height)/2;
    p_win->startx = (COLS - p_win->width)/2;

    p_win->border.ls = '|';
    p_win->border.rs = '|';
    p_win->border.ts = '-';
    p_win->border.bs = '-';
    p_win->border.tl = '+';
    p_win->border.tr = '+';
    p_win->border.bl = '+';
    p_win->border.br = '+';
    //p_win->window = create_newwin(p_win->height, p_win->width, p_win->starty, p_win->startx);
    //wbkgd(p_win->window, COLOR_PAIR(color));
}
void init_win_params(WIN *p_win, int height, int width, int color){
    p_win->height = height;
    p_win->width = width;
    p_win->starty = (LINES - p_win->height)/2;
    p_win->startx = (COLS - p_win->width)/2;

    p_win->border.ls = '|';
    p_win->border.rs = '|';
    p_win->border.ts = '-';
    p_win->border.bs = '-';
    p_win->border.tl = '+';
    p_win->border.tr = '+';
    p_win->border.bl = '+';
    p_win->border.br = '+';
    p_win->window = create_newwin(p_win->height, p_win->width, p_win->starty, p_win->startx);
    wbkgd(p_win->window, COLOR_PAIR(color));
}
void create_box(WIN *p_win, bool flag){
    int i, j;
    int x, y, w, h;
    
    x = p_win->startx;
    y = p_win->starty;
    w = p_win->width;
    h = p_win->height;

    if(flag == TRUE){
        mvaddch(y, x, p_win->border.tl);
        mvaddch(y, x + w, p_win->border.tr);
        mvaddch(y + h, x, p_win->border.bl);
        mvaddch(y + h, x + w, p_win->border.br);
        mvhline(y, x + 1, p_win->border.ts, w - 1);
        mvhline(y + h, x + 1, p_win->border.bs, w - 1);
        mvvline(y + 1, x, p_win->border.ls, h - 1);
        mvvline(y + 1, x + w, p_win->border.rs, h - 1);
    }
    else 
        for(j = y; j <=y+h; ++j)
            for(i = x; i<= x+w; ++i)
                mvaddch(j, i, ' ');

    refresh();
}
