#ifndef TABLEH
#define TABLEH

#include <ncurses.h>
// A TABLE structure, used when a multi-column MENU is needed
typedef struct _table_struct{
    WINDOW *window;             // the underlying window
    int width;                  // window width
    int height;                 // window height
    int num_rows;                // how many rows
    int num_cols;                // how many columns
    int selected_row;            // which row is selected
    int selected_col;            // which column is selected
    char *title;                // window title, displayed at top
    char (*headers)[128];       // column headers
    char (**table_data)[128];    // the data in the table
} TABLE;


void init_table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**table_data)[128]);

void render_table(TABLE *p_table, char (*starred));

void changeselect_table(TABLE *p_table, int changerow, int changecol);
#endif
