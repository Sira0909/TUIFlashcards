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
void editList(char ListName[]){
    char ListPath[FLASHCARDFILESIZE];
    if(ListName[0] == '/' || ListName[0] == '~'){
        strncpy(ListPath, ListName, FLASHCARDFILESIZE);
    }
    else{
        strcpy(ListPath, config.flashcard_dir);
        strncat(ListPath, ListName, FLASHCARDFILESIZE-strnlen(config.flashcard_dir, 128));
    }
    
    FlashcardSet* flashcardset = create_Flashcard_Set_Object();
    
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
    init_table(&flashcardTable, flashcardset->num_items, 2,width, height, &tablewindow, "Editing Flashcards", headers, table);
    wrefresh(flashcardTable.window);
    
    // character from getch()
    int ch;

    // so that we can leave while loop
    bool done = false;


    render_table(&flashcardTable, starred) ;
    box(edit_list_menu_window, 0, 0);
    wmove(edit_list_menu_window, 0, 1); waddch(edit_list_menu_window, ACS_RTEE);wprintw(edit_list_menu_window, "%s", "Editing Flashcards"); waddch(edit_list_menu_window, ACS_LTEE);
    wrefresh(edit_list_menu_window);

    while(!done){
        ch = getch();
        // refresh menu (see MENU.c)
        switch(ch){
            case 'j': // down
                changeselect_table(&flashcardTable, 1, 0);
                break;
            case 'k': // up
                changeselect_table(&flashcardTable, -1, 0);
                break;
            case 'h': // left
                changeselect_table(&flashcardTable, 0, -1);
                break;
            case 'l': // right
                changeselect_table(&flashcardTable, 0, 1);
                break;
            case 's': // star
                flashcardset->cards[flashcardTable.selected_row].is_starred = !flashcardset->cards[flashcardTable.selected_row].is_starred;
                getpairslimiter(flashcardset, starred, items, defns);
                table[0] = items;
                break;
            case 10: //enter
                if(flashcardTable.selected_col == 0){
                    char* Name = getString("Name?", MAX_FLASHCARD_SET_ITEM_SIZE);
                    if(Name != NULL && !is_all_space(Name)){
                        strcpy(flashcardset->cards[flashcardTable.selected_row].name, Name);
                        getpairslimiter(flashcardset, starred, items, defns);
                        table[0] = items;
                        refresh();
                        free(Name);
                    }
                }
                else{
                    char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE);
                    if(Defn != NULL && !is_all_space(Defn)){
                        strcpy(flashcardset->cards[flashcardTable.selected_row].definition, Defn);
                        getpairslimiter(flashcardset, starred, items, defns);
                        table[1] = defns;
                        refresh();
                        free(Defn);
                    }
                }
                break;

            case 'd':
                wattron(edit_list_menu_window, A_BOLD);
                mvwprintw(edit_list_menu_window, height, 1, "really delete? (y/n)");
                wattroff(edit_list_menu_window, A_BOLD);
                wrefresh(edit_list_menu_window);
                if ('y' == getch()){
                    deletecard(flashcardset, flashcardTable.selected_row);
                    free(items);
                    free(defns);
                    items = calloc(flashcardset->capacity, sizeof(char[128]));
                    defns = calloc(flashcardset->capacity, sizeof(char[128]));
                    getpairslimiter(flashcardset, starred, items, defns);
                    table[0] = items;
                    table[1] = defns;
                    flashcardTable.num_rows = flashcardset->num_items;
                    if (flashcardTable.num_rows <= flashcardTable.selected_row){
                        changeselect_table(&flashcardTable, -1, 0);
                    }
                }
                mvwprintw(edit_list_menu_window, height, 1, "                    ");
                break;
            case 'a': {
                char* Name = getString("Name?", MAX_FLASHCARD_SET_ITEM_SIZE);
                if (Name != NULL){
                    if(is_all_space(Name)){
                        free(Name);
                        break;
                    }
                    char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE);
                    if (Defn != NULL){
                        if(is_all_space(Defn)){
                            free(Name);
                            free(Defn);
                            break;
                        }
                        addcard(flashcardset, Name, Defn, 0);
                        free(items);
                        free(defns);
                        items = calloc(flashcardset->capacity, sizeof(char[128]));
                        defns = calloc(flashcardset->capacity, sizeof(char[128]));
                        getpairslimiter(flashcardset, starred, items, defns);
                        table[0] = items;
                        table[1] = defns;
                        flashcardTable.num_rows = flashcardset->num_items;
                        refresh();
                        free(Defn);
                    }
                    free(Name);
                }
                      }
                break;
            case 'w': // write
                wattron(edit_list_menu_window, A_BOLD);
                mvwprintw(edit_list_menu_window, height, 1, "quit after write? (y/n):");
                wattroff(edit_list_menu_window, A_BOLD);
                wrefresh(edit_list_menu_window);
                switch (getch()){
                    case 'y':
                        done = true;
                        erasewindow(flashcardTable.window);
                        erasewindow(edit_list_menu_window);
                        edit_list_menu_window = NULL;
                        flashcardTable.window = NULL;
                        free(flashcardTable.window);
                        flashcardTable.window = NULL;
                        free(items);
                        free(defns);
                        writeFlashcardSet(flashcardset, ListPath, 1);
                        refresh();
                        return;
                    case 'n':
                        writeFlashcardSet(flashcardset, ListPath, 0);
                        mvwprintw(edit_list_menu_window, height, 1, "                            ");
                        break;
                    default:
                        mvwprintw(edit_list_menu_window, height, 1, "Did not write.              ");
                        wrefresh(edit_list_menu_window);
                        continue;
                        break;
                }
                break;
            case 27:
            case 'q': // quit
                wattron(edit_list_menu_window, A_BOLD);
                mvwprintw(edit_list_menu_window, height, 1, "quit without saving? (y/n): ");
                wattroff(edit_list_menu_window, A_BOLD);
                wrefresh(edit_list_menu_window);
                if ('y' == getch())
                    done = true;
                mvwprintw(edit_list_menu_window, height, 1, "                            ");
                break;
            case '?':
                list_keybinds(13, editkeybinds);
                break;

        }
        if(!done){
            render_table(&flashcardTable, starred) ;
            box(edit_list_menu_window, 0, 0);
            wmove(edit_list_menu_window, 0, 1); waddch(edit_list_menu_window, ACS_RTEE);wprintw(edit_list_menu_window, "%s", "Editing Flashcards"); waddch(edit_list_menu_window, ACS_LTEE);
            wrefresh(edit_list_menu_window);
        }

    }
    erasewindow(flashcardTable.window);
    erasewindow(edit_list_menu_window);
    edit_list_menu_window = NULL;
    flashcardTable.window = NULL;
    free(flashcardTable.window);
    flashcardTable.window = NULL;
    free(items);
    free(defns);
    deleteSetPointer(&flashcardset);
    refresh();
}

// Add new flashcard list
void addList(){

    char newfile[FLASHCARDFILESIZE]={0};
    strcpy(newfile, trim_whitespaces(config.flashcard_dir));
    char* file = getString("Name new List", 31);
    if (file == NULL || is_all_space(file)) return;
    strcat(newfile, trim_whitespaces(file));
    FILE* temp = fopen(newfile, "w");
    fclose(temp);
    editList(newfile);
}

