#ifndef TABLEH
#define TABLEH
#include "WIN.h"
#include "constants.h"

typedef struct _table_struct{
    WIN *p_win;
    int numrows;
    int numcols;
    int selectedrow;
    int selectedcol;
    char *title;
    char (*headers)[128];
    char (**tabledata)[128];
} TABLE;

void init_table(TABLE *p_table, int rows, int cols, int width, int height, WINDOW** window, char *title, char (*headers)[128], char (**tabledata)[128]);
void render_table(TABLE *p_table, char (*starred));

void changeselect_table(TABLE *p_table, int changerow, int changecol);
#endif
