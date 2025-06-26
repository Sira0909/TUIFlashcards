
#include "TABLE.h"
#include "helpers.h"
#include <ncurses.h>
#include <string.h>
void init_table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**tabledata)[128]){
    WIN* pwin = malloc(sizeof(WIN)); //init_win_params_simple(p_table->p_win, height, width, 2);
    pwin->height = height;
    pwin->width = width;
    pwin->starty = (LINES - height)/2;
    pwin->startx = (COLS - width)/2;
    pwin->window = *window;
    *p_table = (TABLE){pwin, rows, cols, 0, 0, title, headers, tabledata};
    wattron(*window, COLOR_PAIR(2));
    wbkgd(*window, COLOR_PAIR(2));

}

void render_table(TABLE *p_table, char (*starred)){
    werase(p_table->p_win->window);
    wbkgd(p_table->p_win->window, COLOR_PAIR(2));
    int ideal = (p_table->p_win->height) / 2;
    int center;
    if (p_table->selectedrow < ideal) center = ideal;
    else if(p_table->selectedrow + ideal > p_table->numrows) center = p_table->numrows-ideal;
    else center = p_table->selectedrow;

    int maxlength = (p_table->p_win->width - (p_table->numcols-1))/p_table->numcols;

    for(int i = center - ideal; (i < center - ideal + p_table->p_win->height) && (i < p_table->numrows); i++){
        for (int col = 0; col < p_table->numcols; col ++){
            if (p_table->selectedrow ==i && p_table->selectedcol == col) {
                wattron(p_table->p_win->window, A_BOLD);
                mvwprintw(p_table->p_win->window, i+1,col*(1 + maxlength), "%s", p_table->tabledata[col][i]);
                wattroff(p_table->p_win->window, A_BOLD);
                wmove(p_table->p_win->window, i+1, col*(1+maxlength));
                wchgat(p_table->p_win->window, maxlength,A_BOLD, 3, NULL);
            }
            else if(strlen(p_table->tabledata[col][i]) > maxlength){
                char *item = calloc(p_table->p_win->width, sizeof(char));
                strncpy(item, p_table->tabledata[col][i], maxlength-3);
                strcat(item, "...");
                mvwprintw(p_table->p_win->window, i+1,col*(1 + maxlength), "%s", item);
                wmove(p_table->p_win->window, i+1, col*(1+maxlength));
                wchgat(p_table->p_win->window, maxlength,A_NORMAL, 2, NULL);
            }
            else{
                mvwprintw(p_table->p_win->window, i+1,col*(1 + maxlength), "%s", p_table->tabledata[col][i]);
                wmove(p_table->p_win->window, i+1, col*(1+maxlength));
                wchgat(p_table->p_win->window, maxlength,A_NORMAL, 2, NULL);
            }

            wmove(p_table->p_win->window, i+1, col*(1+maxlength));
            if(starred[i] == '*') {
                if (p_table->selectedrow ==i && p_table->selectedcol == col){
                        wchgat(p_table->p_win->window, maxlength,A_BOLD, 5, NULL);
                }
                else{
                    if(p_table->selectedrow ==i){
                        wchgat(p_table->p_win->window, maxlength,A_BOLD, 4, NULL);
                    }
                    wchgat(p_table->p_win->window, maxlength,A_NORMAL, 4, NULL);
                }
            }

        }
        for (int col = 1; col < p_table->numcols; col ++){
            mvwaddch(p_table->p_win->window, i+1, col*(1+maxlength)-1, ACS_VLINE);
        }
        
    }
    for (int col = 0; col < p_table->numcols; col ++){
        wmove( p_table->p_win->window, 0, col*(1 + maxlength));
        wprintw(p_table->p_win->window, "%s", p_table->headers[col]);
    }
    wmove( p_table->p_win->window, 0, 0);
    wchgat(p_table->p_win->window, p_table->p_win->width,A_BOLD | A_UNDERLINE, 2, NULL);

    wattron(p_table->p_win->window, A_UNDERLINE);
    for (int col = 0; col < p_table->numcols; col ++){
        //mvwaddch(p_table->p_win->window, 0, col*(1+maxlength), ACS_TTEE);
        mvwaddch(p_table->p_win->window, 0, col*(1+maxlength)-1, ACS_VLINE);
    }

    wattroff(p_table->p_win->window, A_UNDERLINE);

    wrefresh(p_table->p_win->window);
    
}
void changeselect_table(TABLE *p_table, int changerow, int changecol){
    p_table->selectedrow += changerow;
    p_table->selectedcol += changecol;
    if (!(p_table->selectedrow<p_table->numrows)){ p_table->selectedrow = p_table->numrows-1; changerow = -1;}
    if (!(p_table->selectedcol<p_table->numcols)){ p_table->selectedcol = p_table->numcols-1; changecol = -1;}
    if (p_table->selectedrow<0) {p_table->selectedrow = 0;changerow = 1;}
    if (p_table->selectedcol<0) {p_table->selectedcol = 0;changecol = 1;}
    while(!strcmp(p_table->tabledata[0][p_table->selectedrow], "\0")){
        p_table->selectedrow += sign(changerow);
        if (!(p_table->selectedrow<p_table->numrows)){ p_table->selectedrow = p_table->numrows-1; changerow = -1;}
    }
}
