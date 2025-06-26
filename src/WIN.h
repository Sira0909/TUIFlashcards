#ifndef WINH
#define WINH
#include "window.h"
typedef struct _win_border_struct {
    chtype ls, rs, ts, bs,
           tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct{
    WINDOW *window;
    int startx, starty;
    int height, width;
    WIN_BORDER border;
}WIN;

void init_win_params(WIN *p_win, int height, int width, int color);

void init_win_params_simple(WIN *p_win, int height, int width, int color);

void create_box(WIN *p_win, bool flag);

#endif
