
#include <macros.h>
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <UI.h>
#include <windows/table.h>


//constructor
void init_Table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**table_data)[128], char* highlighted){
    *p_table = (TABLE){*window, width, height, rows, cols, 0, 0, title, headers, table_data, highlighted, 0, 1, malloc(sizeof(struct hook))};

    // draws background but doesnt render yet
    wattron(*window, COLOR_PAIR(2));
    wbkgd(*window, COLOR_PAIR(2));

}
void addHook_Table(TABLE *p_table, struct hook hook){
    if(p_table->hookcount == p_table->maxhooks){
        p_table->hooks = realloc(p_table->hooks, p_table->maxhooks*2* sizeof(struct hook));
        if(p_table->hooks==NULL){
            printf("error in realloc");
            exit(-1);
        }
        p_table->maxhooks*=2;
    }
    p_table->hooks[p_table->hookcount] = hook;
    p_table->hookcount++;
};

void run_Table(TABLE* p_table){
    while(1){
        render_table(p_table, p_table->highlighted);
        int ch = getch();
        for(int i = 0; i<p_table->hookcount; i++){
            if(ch == p_table->hooks[i].trigger){
                if(p_table->hooks[i].effect(p_table)==-1){
                    return;
                }
            }

        }
    }

}

void changeselect_Table(TABLE *p_table, int changerow, int changecol){
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

int table_down (void* table){changeselect_Table((TABLE*) table, 1,  0); return 1;}
int table_up   (void* table){changeselect_Table((TABLE*) table, -1, 0); return 1;}
int table_left (void* table){changeselect_Table((TABLE*) table, 0, -1); return 1;}
int table_right(void* table){changeselect_Table((TABLE*) table, 0,  1); return 1;}
