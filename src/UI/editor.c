#include <UI.h>
#include <string.h>
#include <macros.h>
#include <flashcards.h>
#include <windows/table.h>
#include <config.h>
#include <windows/window.h>

#include <stdlib.h>
#include <ncurses.h>
#include <form.h>

char editkeybinds[13][2][20]= { 
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "edit text"},
    {" ", " "},
    {"s", "star flashcard"},
    {"a", "add flashcard"},
    {"d", "delete flashcard"},
    {" ", " "},
    {"w", "save"},
    {"q", "quit without save"},
    {"?", "list keybinds"}
};

FlashcardSet* flashcardset;


int edit_j(void* table){changeselect_Table((TABLE*) table, 1,  0); return 1;}
int edit_k(void* table){changeselect_Table((TABLE*) table, -1, 0); return 1;}
int edit_h(void* table){changeselect_Table((TABLE*) table, 0, -1); return 1;}
int edit_l(void* table){changeselect_Table((TABLE*) table, 0,  1); return 1;}
int edit_s(void* table){
                flashcardset->cards[((TABLE*)table)->selected_row].is_starred = !flashcardset->cards[((TABLE*)table)->selected_row].is_starred;
                ((TABLE*)table)->highlighted[((TABLE*)table)->selected_row]=(flashcardset->cards[((TABLE*)table)->selected_row].is_starred)? '*': ' ';
                return 1;
}
int edit_select(void* table){
                TABLE* Table = (TABLE*)table;
                if(Table->selected_col == 0){
                    char* Name = getString("Name?", MAX_FLASHCARD_SET_ITEM_SIZE, flashcardset->cards[(Table)->selected_row].name);
                    if(Name==NULL)
                        return 1;
                    if(is_all_space(Name))
                        free(Name);
                    else if(Name != NULL){
                        strcpy(flashcardset->cards[Table->selected_row].name, Name);
                        getpairslimiter(flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1]);
                        refresh();
                        free(Name);
                    }
                }
                else{
                    char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE, flashcardset->cards[(Table)->selected_row].definition);
                    if(Defn==NULL)
                        return 1;
                    if(is_all_space(Defn))
                        free(Defn);
                    else if(Defn != NULL){
                        strcpy(flashcardset->cards[(Table)->selected_row].definition, Defn);
                        getpairslimiter(flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1]);
                        refresh();
                        free(Defn);
                    }
                }
                return 1;
}
int edit_delete(void* table){
                TABLE* Table = (TABLE*)table;
                wattron(Table->window, A_BOLD);
                mvwprintw(Table->window, Table->height-1, 0, "really delete? (y/n)");
                wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);
                if ('y' == getch()){
                    deletecard(flashcardset, Table->selected_row);
                    free(Table->table_data[0]);
                    free(Table->table_data[1]);
                    free(Table->highlighted);
                    Table->table_data[0] = calloc(flashcardset->capacity, sizeof(char[128]));
                    Table->table_data[1] = calloc(flashcardset->capacity, sizeof(char[128]));
                    Table->highlighted = calloc(flashcardset->capacity, sizeof(char));
                    getpairslimiter(flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1]);
                    Table->num_rows = flashcardset->num_items;
                    if (Table->num_rows <= Table->selected_row){
                        changeselect_Table(Table, -1, 0);
                    }
                }
                mvwprintw(Table->window, Table->height-1, 1, "                    ");
                return 1;
}
int edit_add(void* table){
                TABLE* Table = (TABLE*) table;
                char* Name = getString("Name?", MAX_FLASHCARD_SET_ITEM_SIZE, NULL);
                if (Name != NULL){
                    if(is_all_space(Name)){
                        free(Name);
                        return 1;
                    }
                    char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE, NULL);
                    if (Defn != NULL){
                        if(is_all_space(Defn)){
                            free(Name);
                            free(Defn);
                            return 1;
                        }
                        addcard(flashcardset, Name, Defn, 0);
                        free(Table->table_data[0]);
                        free(Table->table_data[1]);
                        free(Table->highlighted);
                        Table->table_data[0] = calloc(flashcardset->capacity, sizeof(char[128]));
                        Table->table_data[1] = calloc(flashcardset->capacity, sizeof(char[128]));
                        Table->highlighted = calloc(flashcardset->capacity, sizeof(char));
                        getpairslimiter(flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1]);
                        Table->num_rows = flashcardset->num_items;
                        refresh();
                        free(Defn);
                    }
                    free(Name);
                }
                return 1;
}
char* filename;
int edit_write(void* table){
                TABLE* Table = (TABLE*) table;
            wattron(Table->window, A_BOLD);
            mvwprintw(Table->window, Table->height-1, 1, "quit after write? (y/n):");
            wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);
                switch (getch()){
                    case 'y':
                        free(Table->table_data[0]);
                        free(Table->table_data[1]);
                        free(Table->highlighted);
                        writeFlashcardSet(flashcardset, filename, 1);
                        refresh();
                        return -1;
                    case 'n':
                        writeFlashcardSet(flashcardset, filename, 0);
                        mvwprintw(Table->window, Table->height-1, 1, "                            ");
                        return 1;
                    default:
                        mvwprintw(Table->window, Table->height-1, 1, "Did not write.              ");
                        wrefresh(Table->window);
                        return 1;
                }
                return 1;
}
int edit_quit(void* table){
                TABLE* Table = (TABLE*) table;
                wattron(Table->window, A_BOLD);
                mvwprintw(Table->window, Table->height-1, 1, "quit without saving? (y/n): ");
                wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);
                if ('y' == getch()){
                    free(Table->table_data[0]);
                    free(Table->table_data[1]);
                    free(Table->highlighted);
                    deleteSetPointer(&flashcardset);
                    return -1;
                }
                return 1;
}
int edit_keybinds(void* table){
                list_keybinds(13, editkeybinds);
                return 1;
}
void editList(char ListName[]){
    char ListPath[PATH_MAX];
    if(ListName[0] == '/' || ListName[0] == '~'){
        strncpy(ListPath, ListName, PATH_MAX);
    }
    else{
        strcpy(ListPath, config.flashcard_dir);
        strncat(ListPath, ListName, PATH_MAX-strnlen(config.flashcard_dir, 128));
    }
    filename = ListPath;
    
    flashcardset = create_Flashcard_Set_Object();
    
    if (-1 == fillFlashcardSet(flashcardset, ListPath)) {
        deleteSetPointer(&flashcardset);
        return;
    }


    TABLE flashcardTable;

    int height = max(21, LINES - 5);
    int width = 41;
    WINDOW* edit_list_menu_window = create_newwin(height+2, width+2, (LINES - height)/2-1, (COLS - width)/2);
    WINDOW* tablewindow = derwin(edit_list_menu_window, height, width, 1, 1);

    char (*items)[128] = calloc(flashcardset->capacity, sizeof(char[128]));

    char (*defns)[128] = calloc(flashcardset->capacity, sizeof(char[128]));

    char (*starred) = calloc(flashcardset->capacity, sizeof(char));

    getpairslimiter(flashcardset, starred, items, defns);

    char (*(table[2]))[128] = {items, defns};

    char headers[2][128] = {"word", "definition"};

    wbkgd(edit_list_menu_window, COLOR_PAIR(2));
    box(edit_list_menu_window, 0, 0);

    wrefresh(edit_list_menu_window);
    // init the menu
    init_Table(&flashcardTable, flashcardset->num_items, 2,width, height, &tablewindow, "Editing Flashcards", headers, table, starred);
    wrefresh(flashcardTable.window);
    

    render_table(&flashcardTable, starred) ;
    box(edit_list_menu_window, 0, 0);
    wmove(edit_list_menu_window, 0, 1); waddch(edit_list_menu_window, ACS_RTEE);wprintw(edit_list_menu_window, "%s", "Editing Flashcards"); waddch(edit_list_menu_window, ACS_LTEE);
    wrefresh(edit_list_menu_window);

    addHook_Table(&flashcardTable, (struct hook){'h', edit_h });
    addHook_Table(&flashcardTable, (struct hook){'j', edit_j });
    addHook_Table(&flashcardTable, (struct hook){'k', edit_k });
    addHook_Table(&flashcardTable, (struct hook){'l', edit_l });
    addHook_Table(&flashcardTable, (struct hook){'s', edit_s });
    addHook_Table(&flashcardTable, (struct hook){10, edit_select });
    addHook_Table(&flashcardTable, (struct hook){'q', edit_quit });
    addHook_Table(&flashcardTable, (struct hook){27,  edit_quit });
    addHook_Table(&flashcardTable, (struct hook){'?',  edit_keybinds });
    addHook_Table(&flashcardTable, (struct hook){'d',  edit_delete });
    addHook_Table(&flashcardTable, (struct hook){'a',  edit_add });
    addHook_Table(&flashcardTable, (struct hook){'w',  edit_write });

    run_Table(&flashcardTable);

    erasewindow(flashcardTable.window);
    erasewindow(edit_list_menu_window);
    edit_list_menu_window = NULL;
    flashcardTable.window = NULL;
    filename = NULL;
    refresh();
}

// Add new flashcard list
void addList(){

    char newfile[PATH_MAX]={0};
    strcpy(newfile, trim_whitespaces(config.flashcard_dir));

    char* file = getString("Name new List", 31, NULL);
    if (file == NULL) return;
    if(is_all_space(file)){
        free(file);
        return;
    }
    strcat(newfile, trim_whitespaces(file));
    free(file);

    FILE* temp = fopen(newfile, "w");
    fclose(temp);
    editList(newfile);
}

