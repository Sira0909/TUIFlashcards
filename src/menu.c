#include <ncurses.h>
#include <string.h>
#include <macros.h>
#include <windows/menu.h>
#include <UI.h>

void init_Menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char* highlighted, char (*menuitems)[]){
    *p_menu = (MENU){*window, width, height, size, 0, title, menuitems, highlighted, 0, 1};
    wbkgd(p_menu->window, COLOR_PAIR(2));

}




// changes selected item
void changeselect_Menu(MENU *p_menu, int change){
    p_menu->selected += change;

    //overflow checks
    if (!(p_menu->selected<p_menu->numOptions)){ p_menu->selected = p_menu->numOptions-1; change = -1;}
    if (p_menu->selected<0) {p_menu->selected = 0;change = 1;}

    //skip empty
    while(!strcmp(p_menu->menuitems[p_menu->selected], "\0")){
        p_menu->selected += sign(change);
        if (!(p_menu->selected<p_menu->numOptions)){ p_menu->selected = p_menu->numOptions-1; change = -1;}
    }
}
int menu_up   (void* menu){changeselect_Menu((MENU*) menu, -1); return 1;}
int menu_down (void* menu){changeselect_Menu((MENU*) menu,  1); return 1;}
