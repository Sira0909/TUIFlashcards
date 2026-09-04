#ifndef TABLEH
#define TABLEH

#include <ncurses.h>
// A TABLE structure, used when a multi-column MENU is needed
typedef struct _table_struct{
    WINDOW *window;                 // the underlying window
    int width;                      // window width
    int height;                     // window height
    int num_rows;                   // how many rows
    int num_cols;                   // how many columns
    int selected_row;               // which row is selected
    int selected_col;               // which column is selected
    char *title;                    // window title, displayed at top
    char (*headers)[128];           // column headers
    char (**table_data)[128];       // the data in the table
    char *highlighted;
    int hookcount;
    int maxhooks;
    void* metadata;
} TABLE;


void init_Table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**table_data)[128], char* highlighted);



int render_Table(void *menu);

void changeselect_Table(TABLE *p_table, int changerow, int changecol);

int table_down (void* table);
int table_up   (void* table);
int table_left (void* table);
int table_right(void* table);
#endif
