#ifndef MENUH
#define MENUH
#include "WIN.h"
#include "constants.h"

typedef struct _menu_struct{
    WIN *p_win;
    int numOptions;
    int selected;
    char *title;
    char (*menuitems)[128];
} MENU;

void init_menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char (*menuitems)[]);

void render_menu(MENU *p_menu);

void changeselect(MENU *p_menu, int change);
#endif
