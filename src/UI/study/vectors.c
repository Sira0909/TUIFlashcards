#include <study.h>
#include <stdlib.h>
#include <config.h>
#include <ncurses.h>
#include <windows/table.h>
#include <windows/window.h>
#include <UI.h>


char *vectorKeybinds[8][2] = {
    {config.keylayout.str_dkey,"down"},
    {config.keylayout.str_ukey,"up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {"a", "select all"},
    {"c", "deselect all"},
    {" ", " "},
    {"?", "list keybinds"}
};
struct vectorMetadata{
    bool *vectorin;
    bool *vectorout;
    int vectorCount;
};
#define Metadata ((struct vectorMetadata*)(((TABLE*)table)->metadata))


int vector_quit(void* table){return -1;}
int vector_keybinds(void*table){
    list_keybinds(8, vectorKeybinds);                         return 1;
}
int vector_select(void*table){
    TABLE* Table = (TABLE*) table;
    if( Table->selected_col==0){
        Metadata->vectorin[Table->selected_row] = !Metadata->vectorin[Table->selected_row];
        sprintf(Table->table_data[0][Table->selected_row], "[%c] Definition %d", (Metadata->vectorin[Table->selected_row]) ?'x':' ', Table->selected_row);//TODO: support names
    }
    if( Table->selected_col==1){
        Metadata->vectorout[Table->selected_row] = !Metadata->vectorout[Table->selected_row];
        sprintf(Table->table_data[1][Table->selected_row], "[%c] Definition %d", (Metadata->vectorout[Table->selected_row]) ?'x':' ', Table->selected_row);//TODO: support names
    }
    return 1;
}
int vector_all(void*table){
    TABLE* Table = (TABLE*) table;
    for(int i = 0; i < Metadata->vectorCount;i++){
        Metadata->vectorin[Table->selected_row] = true;
        Metadata->vectorout[Table->selected_row] = true;
        sprintf(Table->table_data[0][Table->selected_row], "[x] Definition %d", Table->selected_row);//TODO: support names
        sprintf(Table->table_data[1][Table->selected_row], "[x] Definition %d", Table->selected_row);//TODO: support names
    }
    return 1;
}
int vector_none(void*table){
    TABLE* Table = (TABLE*) table;
    for(int i = 0; i < Metadata->vectorCount;i++){
        Metadata->vectorin[Table->selected_row]  = false;
        Metadata->vectorout[Table->selected_row] = false;
        sprintf(Table->table_data[0][Table->selected_row], "[ ] Definition %d", Table->selected_row);//TODO: support names
        sprintf(Table->table_data[1][Table->selected_row], "[ ] Definition %d", Table->selected_row);//TODO: support names
    }
    return 1;
}
void editVectors(bool *vectorin,bool *vectorout,int vectorCount){
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

        sprintf(items[0][i], "[%c] Definition %d", (selectin) ?'x':' ', i);//TODO: support names
        sprintf(items[1][i], "[%c] Definition %d", (selectout)?'x':' ', i);//TODO: support names
        selected[i]='*';//maybe change
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

     
    // character from getch()

    addHook_Table(&vectorTable, (struct hook){config.keylayout.lkey, &table_left});
    addHook_Table(&vectorTable, (struct hook){config.keylayout.dkey, &table_down});
    addHook_Table(&vectorTable, (struct hook){config.keylayout.ukey, &table_up});
    addHook_Table(&vectorTable, (struct hook){config.keylayout.rkey, &table_right});
    addHook_Table(&vectorTable, (struct hook){27, &vector_quit});
    addHook_Table(&vectorTable, (struct hook){'q', &vector_quit});
    addHook_Table(&vectorTable, (struct hook){'?', &vector_keybinds});
    addHook_Table(&vectorTable, (struct hook){10, &vector_select});
    addHook_Table(&vectorTable, (struct hook){'a',&vector_all});
    addHook_Table(&vectorTable, (struct hook){'c',&vector_none});
    run_Table(    &vectorTable);
    erasewindow(table_window);
    erasewindow(vector_window);
    free(vectorTable.hooks);
    free(items[0]);
    free(items[1]);
}
