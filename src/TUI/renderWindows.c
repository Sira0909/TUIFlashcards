#include <ncurses.h>
#include <windows/menu.h>
#include <windows/table.h>
#include <macros.h>
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
    else if(p_menu->selected + ideal > p_menu->numOptions) center = max(ideal,p_menu->numOptions-ideal);

    // if there are enough columns before and after it, the selected row is placed in the center of the available space
    else center = p_menu->selected;

    for(int i = center - ideal; (i < center - ideal + p_menu->height) && (i < p_menu->numOptions); i++){
        int size = strlen(p_menu->menuitems[i]); 

        // too long 
        if(size > p_menu->width){
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

//draws the table
void render_table(TABLE *p_table, char (*starred)){
    //erases window, resets background
    werase(p_table->window);
    wbkgd(p_table->window, COLOR_PAIR(2));


    // the center of available table space
    int ideal = (p_table->height) / 2;
    int center;
    
    // if there aren't enough columns before the selected row, top of table is top row
    if (p_table->selected_row < ideal) center = ideal;
    
    // if there aren't enough columns after the selected row, bottom of table is bottom row
    else if(p_table->selected_row + ideal > p_table->num_rows) center = max(ideal,p_table->num_rows-ideal+1);

    // if there are enough columns before it, the selected row is placed in the center of the available space
    else center = p_table->selected_row;

    // max length of a column
    int max_length = (p_table->width - (p_table->num_cols-1))/p_table->num_cols;



    for(int i = min(p_table->height, p_table->num_rows)-1; i >= 0; i--){

        for (int col = p_table->num_cols-1; col >= 0; col --){


            // selected item is red
            if (p_table->selected_row ==center - ideal + i && p_table->selected_col == col) {
                
                // allow selected items to wrap arround
                //wattron(p_table->window, A_BOLD | COLOR_PAIR(3));
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", p_table->table_data[col][center - ideal +i]);
                //wattroff(p_table->window, A_BOLD | COLOR_PAIR(3));
                wmove(p_table->window, i+1, col*(1+max_length));
                wchgat(p_table->window, max_length,A_BOLD, 3, NULL);
                
            }

            // if non-selected item is longer than length, trim it to length
            else if(strlen(p_table->table_data[col][center - ideal +i]) > max_length){
                char *item = calloc(p_table->width, sizeof(char));
                strncpy(item, p_table->table_data[col][center - ideal +i], max_length-3);
                strcat(item, "...");
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", item);
                wmove(p_table->window, i+1, col*(1+max_length));
                wchgat(p_table->window, max_length,A_NORMAL, 2, NULL);
                free(item);
            }
            
            // print normally
            else{
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", p_table->table_data[col][center - ideal +i]);
                wmove(p_table->window, i+1, col*(1+max_length));
                if(p_table->selected_row == center-ideal + i){ // bold if in same row as selected
                    wchgat(p_table->window, max_length,A_BOLD, 2, NULL);
                }
                else{
                    wchgat(p_table->window, max_length,A_NORMAL, 2, NULL);
                }
            }

            wmove(p_table->window, i+1, col*(1+max_length));
            
            // starred items are yellow
            if(starred != NULL && starred[center - ideal +i] == '*') {
                // starred selected is yellow on red and bold
                if (p_table->selected_row ==center - ideal +i && p_table->selected_col == col)
                        wchgat(p_table->window, max_length,A_BOLD, 5, NULL);
                
                // starred in same row as selected is bold yellow on white
                else if(p_table->selected_row ==center - ideal +i)
                        wchgat(p_table->window, max_length,A_BOLD, 4, NULL);
                    
                // starred nonselected is yellow on white
                else
                    wchgat(p_table->window, max_length,A_NORMAL, 4, NULL);
                
            }

            mvwaddch(p_table->window, i+1, col*(1+max_length)-1, ACS_VLINE);
        }
        
    }
    
    
    // print headers
    for (int col = 0; col < p_table->num_cols; col ++){
        wmove( p_table->window, 0, col*(1 + max_length));
        wprintw(p_table->window, "%s", p_table->headers[col]);
    }

    //underline line with headers
    wmove( p_table->window, 0, 0);
    wchgat(p_table->window, p_table->width,A_BOLD | A_UNDERLINE, 2, NULL);

    for (int col = 0; col < p_table->num_cols; col ++){
        mvwaddch(p_table->window, 0, col*(1+max_length)-1, ACS_VLINE);
    }
    


    wrefresh(p_table->window);
    
}
