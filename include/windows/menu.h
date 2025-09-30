#ifndef MENUH
#define MENUH

#include <ncurses.h>

typedef struct _menu_struct{
    WINDOW *window;             // the underlying window
    int width;                  // window width
    int height;                 // window height
    int numOptions;
    int selected;
    char *title;
    char (*menuitems)[128];
} MENU;

void init_menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char (*menuitems)[]);

// draws menu
void render_menu(MENU *p_menu, char* highlighted);

// changes selected item
void changeselect(MENU *p_menu, int change);
#endif
