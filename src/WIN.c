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
