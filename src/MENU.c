#include "MENU.h"
#include <ncurses.h>
#include <string.h>
#include "helpers.h"
//constructor
void init_menu(MENU *p_menu, int size, int width, int height, WINDOW** window, char *title, char (*menuitems)[]){
    WIN* pwin = malloc(sizeof(WIN)); //init_win_params_simple(p_menu->p_win, height, width, 2);
    pwin->height = height;
    pwin->width = width;
    pwin->window = *window;
    *p_menu = (MENU){pwin, size, 0, title, menuitems};
    wbkgd(p_menu->p_win->window, COLOR_PAIR(2));

}

// draws menu
void render_menu(MENU *p_menu){
    // the center of available table space
    int ideal = (p_menu->p_win->height) / 2;
    int center;

    // if there aren't enough columns before the selected row, top of table is top row
    if (p_menu->selected < ideal) center = ideal;

    // if there aren't enough columns after the selected row, bottom of table is bottom row
    else if(p_menu->selected + ideal > p_menu->numOptions) center = p_menu->numOptions-ideal;

    // if there are enough columns before and after it, the selected row is placed in the center of the available space
    else center = p_menu->selected;

    for(int i = center - ideal; (i < center - ideal + p_menu->p_win->height) && (i < p_menu->numOptions); i++){
        int size = strlen(p_menu->menuitems[i]); 

        // too long 
        if(strlen(p_menu->menuitems[i]) > p_menu->p_win->width){
            char *item = calloc(p_menu->p_win->width, sizeof(char));
            strncpy(item, p_menu->menuitems[i], p_menu->p_win->width-3);
            // to represent too long
            strcat(item, "...");

            mvwprintw(p_menu->p_win->window, i+1,1, "%s", item);
            wmove(p_menu->p_win->window, i+1, 1);
            free(item);

            //selected is bold and red highlight
            if(p_menu->selected ==i) 
                wchgat(p_menu->p_win->window, -1,A_BOLD, 3, NULL);
            else
                wchgat(p_menu->p_win->window, -1,A_NORMAL, 2, NULL);


        }
        mvwprintw(p_menu->p_win->window, i+1,1, "%s", p_menu->menuitems[i]);
        wmove(p_menu->p_win->window, i+1, 1);

        //selected is bold and red highlight
        if(p_menu->selected ==i) 
            wchgat(p_menu->p_win->window, -1,A_BOLD, 3, NULL);
        else
            wchgat(p_menu->p_win->window, -1,A_NORMAL, 2, NULL);

    }
    box(p_menu->p_win->window, 0, 0);
    wmove(p_menu->p_win->window, 0, 1); waddch(p_menu->p_win->window, ACS_RTEE);wprintw(p_menu->p_win->window, "%s", p_menu->title); waddch(p_menu->p_win->window, ACS_LTEE);
    wrefresh(p_menu->p_win->window);
    
}

// changes selected item
void changeselect(MENU *p_menu, int change){
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
