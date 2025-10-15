
#include <macros.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <windows/table.h>


//constructor
void init_table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**table_data)[128]){
    *p_table = (TABLE){*window, width, height, rows, cols, 0, 0, title, headers, table_data};
    wattron(*window, COLOR_PAIR(2));
    wbkgd(*window, COLOR_PAIR(2));
    // draws background but doesnt render yet

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
    else if(p_table->selected_row + ideal > p_table->num_rows) center = p_table->num_rows-ideal;

    // if there are enough columns before it, the selected row is placed in the center of the available space
    else center = p_table->selected_row;

    // max length of a column
    int max_length = (p_table->width - (p_table->num_cols-1))/p_table->num_cols;
    int overwrite = 0;




    // print headers
    for (int col = 0; col < p_table->num_cols; col ++){
        wmove( p_table->window, 0, col*(1 + max_length));
        wprintw(p_table->window, "%s", p_table->headers[col]);
    }
    //underline line with headers
    wmove( p_table->window, 0, 0);
    wchgat(p_table->window, p_table->width,A_BOLD | A_UNDERLINE, 2, NULL);

    wattron(p_table->window, A_UNDERLINE);
    for (int col = 0; col < p_table->num_cols; col ++){
        //mvwaddch(p_table->window, 0, col*(1+max_length), ACS_TTEE);
        mvwaddch(p_table->window, 0, col*(1+max_length)-1, ACS_VLINE);
    }

    wattroff(p_table->window, A_UNDERLINE);
    for(int i = min(center - ideal + p_table->height, p_table->num_rows)-1; i >= center - ideal ; i--){
        for (int col = p_table->num_cols-1; col >= 0; col --){


            // selected item is red
            if (p_table->selected_row ==i && p_table->selected_col == col) {
                // allow selected items to wrap arround
                wattron(p_table->window, A_BOLD);
                wattron(p_table->window, COLOR_PAIR(3));
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", p_table->table_data[col][i]);
                wattroff(p_table->window, COLOR_PAIR(3));
                wattroff(p_table->window, A_BOLD);
                wmove(p_table->window, i+1, col*(1+max_length));
                wchgat(p_table->window, max_length,A_BOLD, 3, NULL);
            }

            // if non-selected item is longer than length, trim it to length
            else if(strlen(p_table->table_data[col][i]) > max_length){
                char *item = calloc(p_table->width, sizeof(char));
                strncpy(item, p_table->table_data[col][i], max_length-3);
                strcat(item, "...");
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", item);
                wmove(p_table->window, i+1, col*(1+max_length));
                wchgat(p_table->window, max_length,A_NORMAL, 2, NULL);
                free(item);
            }
            
            // print normally
            else{
                mvwprintw(p_table->window, i+1,col*(1 + max_length), "%s", p_table->table_data[col][i]);
                wmove(p_table->window, i+1, col*(1+max_length));
                wchgat(p_table->window, max_length,A_NORMAL, 2, NULL);
            }

            wmove(p_table->window, i+1, col*(1+max_length));
            
            // starred items are yellow
            if(starred != NULL && starred[i] == '*') {
                // starred selected is yellow on red and bold
                if (p_table->selected_row ==i && p_table->selected_col == col)
                        wchgat(p_table->window, max_length,A_BOLD, 5, NULL);
                
                // starred in same row as selected is bold yellow on white
                else if(p_table->selected_row ==i)
                        wchgat(p_table->window, max_length,A_BOLD, 4, NULL);
                    
                // starred nonselected is yellow on white
                else
                    wchgat(p_table->window, max_length,A_NORMAL, 4, NULL);
                
            }

            mvwaddch(p_table->window, i+1, col*(1+max_length)-1, ACS_VLINE);
        }
        
    }


    wrefresh(p_table->window);
    
}
void changeselect_table(TABLE *p_table, int changerow, int changecol){
    p_table->selected_row += changerow;
    p_table->selected_col += changecol;

    // dont overflow checks
    if (!(p_table->selected_row<p_table->num_rows)){ p_table->selected_row = p_table->num_rows-1; changerow = -1;}          
    if (!(p_table->selected_col<p_table->num_cols)){ p_table->selected_col = p_table->num_cols-1; changecol = -1;}
    if (p_table->selected_row<0) {p_table->selected_row = 0;changerow = 1;}
    if (p_table->selected_col<0) {p_table->selected_col = 0;changecol = 1;}

    // skip empty items
    while(!strcmp(p_table->table_data[0][p_table->selected_row], "\0")){                                                    
        p_table->selected_row += sign(changerow);
        // overflow check
        if (!(p_table->selected_row<p_table->num_rows)){ p_table->selected_row = p_table->num_rows-1; changerow = -1;}
    }
}

