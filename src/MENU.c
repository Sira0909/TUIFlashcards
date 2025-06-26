#include "MENU.h"
#include <ncurses.h>
#include <string.h>
void init_menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char (*menuitems)[]){
    WIN* pwin = malloc(sizeof(WIN)); //init_win_params_simple(p_menu->p_win, height, width, 2);
    pwin->height = height;
    pwin->width = width;
    pwin->window = *window;
    *p_menu = (MENU){pwin, size, 0, title, menuitems};
    wbkgd(p_menu->p_win->window, COLOR_PAIR(2));

}

void render_menu(MENU *p_menu){
    int ideal = (p_menu->p_win->height) / 2;
    int center;
    if (p_menu->selected < ideal) center = ideal;
    else if(p_menu->selected + ideal > p_menu->numOptions) center = p_menu->numOptions-ideal;
    else center = p_menu->selected;

    for(int i = center - ideal; (i < center - ideal + p_menu->p_win->height) && (i < p_menu->numOptions); i++){
        int size = strlen(p_menu->menuitems[i]); 
        if(strlen(p_menu->menuitems[i]) > p_menu->p_win->width){
            char *item = calloc(p_menu->p_win->width, sizeof(char));
            strncpy(item, p_menu->menuitems[i], p_menu->p_win->width-3);
            strcat(item, "...");

            mvwprintw(p_menu->p_win->window, i+1,1, "%s", item);
            wmove(p_menu->p_win->window, i+1, 1);
            if(p_menu->selected ==i) 
                wchgat(p_menu->p_win->window, -1,A_BOLD, 3, NULL);
            else
                wchgat(p_menu->p_win->window, -1,A_NORMAL, 2, NULL);

        }
        mvwprintw(p_menu->p_win->window, i+1,1, "%s", p_menu->menuitems[i]);
        wmove(p_menu->p_win->window, i+1, 1);
        if(p_menu->selected ==i) 
            wchgat(p_menu->p_win->window, -1,A_BOLD, 3, NULL);
        else
            wchgat(p_menu->p_win->window, -1,A_NORMAL, 2, NULL);

    }
    box(p_menu->p_win->window, 0, 0);
    wmove(p_menu->p_win->window, 0, 1); waddch(p_menu->p_win->window, ACS_RTEE);wprintw(p_menu->p_win->window, "%s", p_menu->title); waddch(p_menu->p_win->window, ACS_LTEE);
    wrefresh(p_menu->p_win->window);
    
}
void changeselect(MENU *p_menu, int change){
    p_menu->selected += change;
    if (!(p_menu->selected<p_menu->numOptions)){ p_menu->selected = p_menu->numOptions-1; change = -1;}
    if (p_menu->selected<0) {p_menu->selected = 0;change = 1;}
    while(!strcmp(p_menu->menuitems[p_menu->selected], "\0")){
        p_menu->selected += sign(change);
        if (!(p_menu->selected<p_menu->numOptions)){ p_menu->selected = p_menu->numOptions-1; change = -1;}
    }
}
