#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <macros.h>
#include <windows/menu.h>
#include <UI.h>

void init_Menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char* highlighted, char (*menuitems)[]){
    *p_menu = (MENU){*window, width, height, size, 0, title, menuitems, highlighted, 0, 1, malloc(sizeof(struct hook))};
    wbkgd(p_menu->window, COLOR_PAIR(2));

}


void addHook_Menu(MENU *p_menu, struct hook hook){
    if(p_menu->hookcount == p_menu->maxhooks){
        p_menu->hooks = realloc(p_menu->hooks, p_menu->maxhooks*2* sizeof(struct hook));
        if(p_menu->hooks==NULL){
            printf("error in realloc");
            exit(-1);
        }
        p_menu->maxhooks*=2;
    }
    p_menu->hooks[p_menu->hookcount] = hook;
    p_menu->hookcount++;
};

void run_Menu(MENU* p_menu){
    while(1){
        render_Menu(p_menu, p_menu->highlighted);
        int ch = getch();
        for(int i = 0; i<p_menu->hookcount; i++){
            if(ch == p_menu->hooks[i].trigger){
                if(p_menu->hooks[i].effect(p_menu)==-1){
                    return;
                }
            }

        }
    }

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
