#ifndef TABLEH
#define TABLEH
#include "WIN.h"
#include "constants.h"

// A TABLE structure, used when a multi-column MENU is needed
typedef struct _table_struct{
    WIN *p_win;                 // the underlying window
    int num_rows;                // how many rows
    int num_cols;                // how many columns
    int selected_row;            // which row is selected
    int selected_col;            // which column is selected
    char *title;                // window title, displayed at top
    char (*headers)[128];       // column headers
    char (**table_data)[128];    // the data in the table
} TABLE;


// constructor
void init_table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**table_data)[128]);

// draws the table. starred can be NULL 
void render_table(TABLE *p_table, char (*starred));

// changes selection in the table. changerow down, changecol right
void changeselect_table(TABLE *p_table, int change_row_by, int change_col_by);
#endif
