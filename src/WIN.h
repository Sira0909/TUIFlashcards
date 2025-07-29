#ifndef WINH
#define WINH
#include "window.h"

// definitions and meathods for WINs, more complex windows

//stores border of window
typedef struct _win_border_struct {
    chtype ls, rs, ts, bs,
           tl, tr, bl, br;
}WIN_BORDER;

//slightly more advanced window. most complex window structures, like MENUs, TABLEs, etc, use a WIN for the actual windows
typedef struct _WIN_struct{
    WINDOW *window;
    int startx, starty;
    int height, width;
    WIN_BORDER border;
}WIN;

// constructs a WIN and draws it
void init_win_params(WIN *p_win, int height, int width, int color);

// constructs a WIN and doesn't draw it
void init_win_params_simple(WIN *p_win, int height, int width, int color);

#endif
