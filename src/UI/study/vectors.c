#include <study.h>
#include <stdlib.h>
#include <config.h>
#include <ncurses.h>
#include <windows/table.h>
#include <windows/window.h>
#include <UI.h>


struct vectorMetadata{
    bool *vectorin;
    bool *vectorout;
    int vectorCount;
};
#define Metadata ((struct vectorMetadata*)(((TABLE*)table)->metadata))


int vector_quit(__attribute__((unused)) void* table){return -1;}
int vector_select(void*table){
    TABLE* Table = (TABLE*) table;
    if( Table->selected_col==0){
        Metadata->vectorin[Table->selected_row] = !Metadata->vectorin[Table->selected_row];
        Table->table_data[0][Table->selected_row][1] = (Metadata->vectorin[Table->selected_row]) ?'x':' ';
    }
    if( Table->selected_col==1){
        Metadata->vectorout[Table->selected_row] = !Metadata->vectorout[Table->selected_row];
        Table->table_data[1][Table->selected_row][1] = (Metadata->vectorout[Table->selected_row]) ?'x':' ';
    }
    return 1;
}
int vector_all(void*table){
    TABLE* Table = (TABLE*) table;
    for(int i = 0; i < Metadata->vectorCount;i++){
        Metadata->vectorin[i] = true;
        Metadata->vectorout[i] = true;
        Table->table_data[0][i][1] = 'x';
        Table->table_data[1][i][1] = 'x';
    }
    return 1;
}
int vector_none(void*table){
    TABLE* Table = (TABLE*) table;
    for(int i = 0; i < Metadata->vectorCount;i++){
        Metadata->vectorin[i]  = false;
        Metadata->vectorout[i] = false;
        Table->table_data[0][i][1] = ' ';
        Table->table_data[1][i][1] = ' ';
    }
    return 1;
}
void editVectors(bool *vectorin,bool *vectorout,int vectorCount, char (*defnNames)[128]){
    struct vectorMetadata meta = {vectorin,vectorout,vectorCount};
    TABLE vectorTable;

    char headers[2][128] = {"_____->term","term->_____"};
    char (*(items[2]))[128];
    items[0]=calloc(vectorCount, sizeof(char[128]));
    items[1]=calloc(vectorCount, sizeof(char[128]));
    char selected[vectorCount];
    for(int i = 0; i< vectorCount; i++){
        bool selectin=vectorin[i];
        bool selectout=vectorout[i];

        sprintf(items[0][i], "[%c] %s", (selectin) ?'x':' ', defnNames[i]);
        sprintf(items[1][i], "[%c] %s", (selectout)?'x':' ', defnNames[i]);
        selected[i]=1;//maybe change
    }



    int width = 43; //1+21*2
    int height = min(vectorCount+4, LINES-4);
    WINDOW *vector_window, *table_window;
    vector_window = create_newwin(height+2, width+2, (LINES - height)/2, (COLS - width)/2);
    table_window = derwin(vector_window,height, width, 1, 1);
    

    init_Table(&vectorTable, vectorCount, 2, width, height, &table_window, "Vectors", headers, items, selected);
    vectorTable.metadata = &meta;
    
    wattron(vector_window, COLOR_PAIR(2));
    wbkgd(vector_window, COLOR_PAIR(2));
    box(vector_window, 0, 0);
    wrefresh(vector_window);

     
    bind_keys(vectorKeybinds, render_Table, 12)
        {config.keylayout.dkey,config.keylayout.str_dkey,"down", &table_down},
        {config.keylayout.ukey,config.keylayout.str_ukey,"up",   &table_up},
        {config.keylayout.lkey,config.keylayout.str_lkey,"left", &table_left},
        {config.keylayout.rkey,config.keylayout.str_rkey,"right",&table_right},
        {-1, " ", " ",NULL},
        {10, "<enter>", "toggle", &vector_select},
        {'a',"a", "select all", &vector_all},
        {'c',"c", "deselect all", &vector_none},
        {-1," ", " ",NULL},
        {'q', "q", "quit",&quit},
        {27, "?", "list keybinds",quit}
    };

    run(&vectorTable,vectorKeybinds);
    erasewindow(table_window);
    erasewindow(vector_window);
    free(items[0]);
    free(items[1]);
}
