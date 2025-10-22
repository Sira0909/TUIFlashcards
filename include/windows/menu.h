#ifndef MENUH
#define MENUH

#include <ncurses.h>
#include <UI.h>

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
    struct hook *hooks;

} MENU;

void init_Menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char* highlighted, char (*menuitems)[]);

void addHook_Menu(MENU *p_menu, struct hook hook);

void run_Menu(MENU* p_menu);

// draws menu
void render_Menu(MENU *p_menu, char* highlighted);

// changes selected item
void changeselect_Menu(MENU *p_menu, int change);
#endif
