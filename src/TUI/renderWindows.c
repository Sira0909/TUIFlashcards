#include <ncurses.h>
#include <windows/menu.h>
#include <string.h>
#include <stdlib.h>
// draws menu
void render_Menu(MENU *p_menu, char* highlighted){
    // the center of available table space
    int ideal = (p_menu->height) / 2;
    int center;

    // if there aren't enough columns before the selected row, top of table is top row
    if (p_menu->selected < ideal) center = ideal;

    // if there aren't enough columns after the selected row, bottom of table is bottom row
    else if(p_menu->selected + ideal > p_menu->numOptions) center = p_menu->numOptions-ideal;

    // if there are enough columns before and after it, the selected row is placed in the center of the available space
    else center = p_menu->selected;

    for(int i = center - ideal; (i < center - ideal + p_menu->height) && (i < p_menu->numOptions); i++){
        int size = strlen(p_menu->menuitems[i]); 

        // too long 
        if(strlen(p_menu->menuitems[i]) > p_menu->width){
            char *item = calloc(p_menu->width, sizeof(char));
            strncpy(item, p_menu->menuitems[i], p_menu->width-3);
            // to represent too long
            strcat(item, "...");

            mvwprintw(p_menu->window, i+1,1, "%s", item);
            wmove(p_menu->window, i+1, 1);
            free(item);

            //selected is bold and red highlight
            if(p_menu->selected ==i) 
                wchgat(p_menu->window, -1,A_BOLD, 3, NULL);
            else
                wchgat(p_menu->window, -1,A_NORMAL, 2, NULL);


        }
        mvwprintw(p_menu->window, i+1,1, "%s", p_menu->menuitems[i]);
        wmove(p_menu->window, i+1, 1);

        //selected is bold and red highlight
        if(p_menu->selected ==i) 
            wchgat(p_menu->window, -1,A_BOLD, 3, NULL);
        else
            wchgat(p_menu->window, -1,A_NORMAL, 2, NULL);
        // starred items are yellow
        if(highlighted != NULL && highlighted[i] == '*') {
            // starred selected is yellow on red and bold
            if (p_menu->selected ==i)
                    wchgat(p_menu->window, -1,A_BOLD, 5, NULL);
                
            // starred nonselected is yellow on white
            else
                wchgat(p_menu->window, -1 ,A_NORMAL, 4, NULL);
            
        }

    }
    box(p_menu->window, 0, 0);
    wmove(p_menu->window, 0, 1); waddch(p_menu->window, ACS_RTEE);wprintw(p_menu->window, "%s", p_menu->title); waddch(p_menu->window, ACS_LTEE);
    wrefresh(p_menu->window);
    
}
