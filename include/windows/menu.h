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
    char *highlighted;
    int hookcount;
    int maxhooks;
    void* metadata;
} MENU;

void init_Menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char* highlighted, char (*menuitems)[]);



// draws menu
int render_Menu(void *menu);

// changes selected item
void changeselect_Menu(MENU *p_menu, int change);

int menu_up   (void* menu);
int menu_down (void* menu);
#endif
