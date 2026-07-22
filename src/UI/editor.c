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
#include <sys/stat.h>

char editkeybinds[15][2][20]= { 
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "edit text"},
    {" ", " "},
    {"s", "star flashcard"},
    {"a", "add flashcard"},
    {"A", "add new definitions"},
    {"d", "delete flashcard"},
    {"D", "remove definitions"},
    {" ", " "},
    {"w", "save"},
    {"q", "quit without save"},
    {"?", "list keybinds"}
};

// set and unset every time the main editor function is run
struct EditorMetadata {
    char* filename;
    FlashcardSet* flashcardset;
};
#define Metadata ((struct EditorMetadata*)(((TABLE*)table)->metadata))

#define CurrentCard flashcardset->cards[Table->selected_row]

//for updating header
int editor_update(void* table); 
//functions
int editor_star(void* table);
int editor_selectField(void* table);
int editor_deleteCard(void* table);
int editor_addCard(void* table);
int editor_addDefinition(void* table);
int editor_writeSet(void* table);
int editor_quit(void* table);
int editor_showkeybinds(void* table);
int editor_removeDefinition(void* table);
void _editList(FlashcardSet* flashcardset, struct EditorMetadata metadata ){
    int columns = flashcardset->num_columns;

    if (columns<2) return;

    TABLE flashcardTable;

    int height = min(51, LINES - 5); // this way always big enough
    int width = 21*columns-1;
    WINDOW* edit_list_menu_window = create_newwin(height+2, width+2, (LINES - height)/2-1, (COLS - width)/2);
    WINDOW* tablewindow = derwin(edit_list_menu_window, height, width, 1, 1);

    char (*items)[128] = calloc(metadata.flashcardset->capacity, sizeof(char[128]));

    char (*(defns[columns-1]))[128]; 
    for(int i = 0 ; i < columns-1;i++){
        defns[i]=calloc(metadata.flashcardset->capacity, sizeof(char[128]));
    }

    char (*starred) = calloc(metadata.flashcardset->capacity, sizeof(char));

    getpairslimiter(metadata.flashcardset, starred, items, defns[0],0);
    for(int i = 1; i < columns-1; i++){
        getDefinitionList(metadata.flashcardset, i, defns[i]);
    }

    char (*(table[columns]))[128];
    table[0]=items;
    for(int i = 1; i < columns; i++){
        table[i]=defns[i-1];
    }
    

    char headers[columns][128];
    strcpy(headers[0],"term");
    for(int i = 1; i < columns; i++){
        //TODO: support for definition names
        char str[20];//just in case someone wants 4,000,000,000 definitions for some reason. gotta support that /sarcasm
        sprintf(str, "definition %d", i);
        strcpy(headers[i],str);
    }

    wbkgd(edit_list_menu_window, COLOR_PAIR(2));
    box(edit_list_menu_window, 0, 0);

    wrefresh(edit_list_menu_window);
    // init the menu
    init_Table(&flashcardTable, metadata.flashcardset->num_items, columns,width, height, &tablewindow, "Editing Flashcards", headers, table, starred);
    flashcardTable.metadata=&metadata;
    wrefresh(flashcardTable.window);
    

    render_Table(&flashcardTable, starred) ;
    box(edit_list_menu_window, 0, 0);
    printProgress(edit_list_menu_window, 0, flashcardset->num_items);

    //add title
    wmove(edit_list_menu_window, 0, 1); waddch(edit_list_menu_window, ACS_RTEE);wprintw(edit_list_menu_window, "%s", "Editing Flashcards"); waddch(edit_list_menu_window, ACS_LTEE);
    wrefresh(edit_list_menu_window);

    addHook_Table(&flashcardTable, (struct hook){'h', table_left });
    addHook_Table(&flashcardTable, (struct hook){'j', table_down });
    addHook_Table(&flashcardTable, (struct hook){'k', table_up });
    addHook_Table(&flashcardTable, (struct hook){'l', table_right });
    addHook_Table(&flashcardTable, (struct hook){'j', editor_update });
    addHook_Table(&flashcardTable, (struct hook){'k', editor_update });
    addHook_Table(&flashcardTable, (struct hook){'s', editor_star });
    addHook_Table(&flashcardTable, (struct hook){10, editor_selectField });
    addHook_Table(&flashcardTable, (struct hook){'q', editor_quit });
    addHook_Table(&flashcardTable, (struct hook){27,  editor_quit });
    addHook_Table(&flashcardTable, (struct hook){'?',  editor_showkeybinds });
    addHook_Table(&flashcardTable, (struct hook){'a',  editor_addCard });
    addHook_Table(&flashcardTable, (struct hook){'A',  editor_addDefinition });
    addHook_Table(&flashcardTable, (struct hook){'d',  editor_deleteCard });
    addHook_Table(&flashcardTable, (struct hook){'D',  editor_removeDefinition });
    addHook_Table(&flashcardTable, (struct hook){'w',  editor_writeSet });

    run_Table(&flashcardTable);

    free(flashcardTable.hooks);
    for(int i=0; i<flashcardTable.num_cols;i++){
            free(flashcardTable.table_data[i]);
    }
    free(flashcardTable.highlighted);
    //cleanup
    erasewindow(flashcardTable.window);
    erasewindow(edit_list_menu_window);
    edit_list_menu_window = NULL;
    flashcardTable.window = NULL;
    refresh();
}
int editor_update(void*table){
    TABLE* Table = (TABLE*)table;
    printProgress(wgetparent(Table->window), Table->selected_row, Metadata->flashcardset->num_items);
    wrefresh(wgetparent(Table->window));
    return 1;
}

int editor_addDefinition(void*table){
    TABLE* Table = (TABLE*)table;
    wbkgd(wgetparent(Table->window), COLOR_PAIR(1));
    werase(wgetparent(Table->window));
    wrefresh(wgetparent(Table->window));
    if(addDefn(Metadata->flashcardset, NULL)==-1){
        //TODO: proper error handling
        endwin();
        exit(-1);
    }

    _editList(Metadata->flashcardset, *Metadata);
    return -1;
}
int editor_removeDefinition(void*table){
    TABLE* Table = (TABLE*)table;
    if(Table->selected_col !=0){
        wbkgd(wgetparent(Table->window), COLOR_PAIR(1));
        werase(wgetparent(Table->window));
        wrefresh(wgetparent(Table->window));
        if(delDefn(Metadata->flashcardset, Table->selected_col-1)==-1){
            //TODO: proper error handling
            endwin();
            exit(-1);
        }

        _editList(Metadata->flashcardset, *Metadata);
        return -1;
    }
    else{
        //TODO: add handling
        return 1;
    }
}
int editor_star(void* table){
                TABLE* Table = (TABLE*)table;
                Metadata->CurrentCard.is_starred = !Metadata->CurrentCard.is_starred;
                Table->highlighted[Table->selected_row]=(Metadata->CurrentCard.is_starred)? '*': ' ';
                return 1;
}
int editor_selectField(void* table){
                TABLE* Table = (TABLE*)table;
                if(Table->selected_col == 0){
                    // edit the term
                    char* Term = getString("Term?", MAX_FLASHCARD_SET_ITEM_SIZE, Metadata->flashcardset->cards[(Table)->selected_row].term);
                    // if empty or cancelled, dont change anything
                    if(Term==NULL)
                        return 1;
                    if(is_all_space(Term))
                        free(Term);
                    else if(Term != NULL){
                        // change set; update table
                        strcpy(Metadata->flashcardset->cards[Table->selected_row].term, Term);
                        getpairslimiter(Metadata->flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1],0);
                        refresh();
                        free(Term);
                    }
                }
                else{
                    // edit the defnition
                    char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE, Metadata->flashcardset->cards[(Table)->selected_row].definition[Table->selected_col-1]);
                    // if empty or cancelled, dont change anything
                    if(Defn==NULL)
                        return 1;
                    if(is_all_space(Defn))
                        free(Defn);
                    else if(Defn != NULL){
                        // change set; update table
                        strcpy(Metadata->flashcardset->cards[(Table)->selected_row].definition[Table->selected_col-1], Defn);
                        getDefinitionList(Metadata->flashcardset, Table->selected_col-1, Table->table_data[Table->selected_col]);
                        refresh();
                        free(Defn);
                    }
                }
                return 1;
}
int editor_deleteCard(void* table){
                TABLE* Table = (TABLE*)table;

                //get confirmation
                wattron(Table->window, A_BOLD);
                mvwprintw(Table->window, Table->height-1, 0, "really delete? (y/n)");
                wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);

                if ('y' == getch()){
                    deletecard(Metadata->flashcardset, Table->selected_row);

                    // update table
                    for(int i=0; i<Table->num_cols;i++){
                            free(Table->table_data[i]);
                            Table->table_data[i]=calloc(Metadata->flashcardset->num_items, sizeof(char[128]));
                    }
                    free(Table->highlighted);
                    Table->highlighted = calloc(Metadata->flashcardset->capacity, sizeof(char));
                    getpairslimiter(Metadata->flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1],0);
                    for(int i=2; i<Table->num_cols;i++){
                        getDefinitionList(Metadata->flashcardset, i-1, Table->table_data[i]);
                    }
                    Table->num_rows = Metadata->flashcardset->num_items;
                    // if that was the bottommost card, move all down.
                    if (Table->num_rows <= Table->selected_row){
                        changeselect_Table(Table, -1, 0);
                    }
                }
                printProgress(wgetparent(Table->window), Table->selected_row, Metadata->flashcardset->num_items);
                wrefresh(wgetparent(Table->window));
                mvwprintw(Table->window, Table->height-1, 1, "                    ");
                return 1;
}
int editor_addCard(void* table){
    //TODO: make this add for all defns
                TABLE* Table = (TABLE*) table;

                //get term 
                char* Term = getString("Term?", MAX_FLASHCARD_SET_ITEM_SIZE, NULL);
                //ensure not empty/cancelled
                if (Term != NULL){
                    if(is_all_space(Term)){
                        free(Term);
                        return 1;
                    }
                    // get definition
                    char Defns[Metadata->flashcardset->num_columns][MAX_FLASHCARD_SET_DEFN_SIZE];
                    strcpy(Defns[0],Term);
                    free(Term);
                    for(int i = 1; i < Metadata->flashcardset->num_columns;i++){
                        char* Defn = getString("Definition?", MAX_FLASHCARD_SET_DEFN_SIZE, NULL);
                        //ensure not empty/cancelled
                        if(Defn == NULL){
                            return 1;
                        }
                        if (Defn != NULL && is_all_space(Defn)){
                                free(Defn);
                                return 1;
                        }
                        strcpy(Defns[i],Defn);
                        free(Defn);
                    }
                    //update
                    addcard(Metadata->flashcardset, Defns[0], Defns+1, 0);
                    for(int i=0; i<Table->num_cols;i++){
                            free(Table->table_data[i]);
                            Table->table_data[i]=calloc(Metadata->flashcardset->num_items, sizeof(char[128]));
                    }
                    free(Table->highlighted);
                    Table->highlighted = calloc(Metadata->flashcardset->capacity, sizeof(char));
                    getpairslimiter(Metadata->flashcardset, Table->highlighted, Table->table_data[0], Table->table_data[1],0);
                    for(int i=2; i<Table->num_cols;i++){
                        getDefinitionList(Metadata->flashcardset, i-1, Table->table_data[i]);
                    }
                    Table->num_rows = Metadata->flashcardset->num_items;
                    refresh();

                }
                printProgress(wgetparent(Table->window), Table->selected_row, Metadata->flashcardset->num_items);
                wrefresh(wgetparent(Table->window));
                return 1;
}

int editor_writeSet(void* table){
            TABLE* Table = (TABLE*) table;
            wattron(Table->window, A_BOLD);
            mvwprintw(Table->window, Table->height-1, 1, "quit after write? (y/n):");
            wattroff(Table->window, A_BOLD);
            wrefresh(Table->window);
            switch (getch()){
                case 'y':
                    //clean up
                    writeFlashcardSet(Metadata->flashcardset, Metadata->filename, 1);
                    refresh();
                    //end
                    return -1;
                case 'n':
                    //write
                    writeFlashcardSet(Metadata->flashcardset, Metadata->filename, 0);
                    mvwprintw(Table->window, Table->height-1, 1, "                            ");
                    return 1;
                default:
                    //cancel
                    mvwprintw(Table->window, Table->height-1, 1, "Did not write.              ");
                    wrefresh(Table->window);
                    return 1;
            }
            return 1;
}
int editor_quit(void* table){
                TABLE* Table = (TABLE*) table;
                wattron(Table->window, A_BOLD);
                //confirm
                mvwprintw(Table->window, Table->height-1, 1, "quit without saving? (y/n): ");
                wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);
                if ('y' == getch()){
                    //clean up and quit
                    deleteSetPointer(&Metadata->flashcardset);
                    return -1;
                }
                return 1;
}
int editor_showkeybinds(void* table){
                list_keybinds(15, editkeybinds);
                return 1;
}

// Add new flashcard list
void addList(char* dir){

    char newfile[PATH_MAX]={0};
    strcpy(newfile, trim_whitespaces(dir));
    //strcat(newfile, "/");

    char* file = getString("Name new List", 31, NULL);
    if (file == NULL) return;
    if(is_all_space(file)){
        free(file);
        return;
    }
    
    int create = 0;
 
    for(int i = 0; i < strnlen(file,31); i++){
        if(file[i]=='/'){
            if(getConfirmation("Did you mean to put this list into a folder?", NULL, "List creation canceled")){
                create = 2;
            }
            else{
                create = 1;
            }
            break;

        }

    }
    if(create==1){return;}
    
    strcat(newfile, trim_whitespaces(file));
    if(create==2){
        for(int i = strlen(dir)+1;i<strnlen(newfile,PATH_MAX);i++){
            if(newfile[i]=='/'){
                newfile[i]='\0';
                if(mkdir(newfile,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)==-1){showmsg("Failed to create directory");return;}
                newfile[i]='/';
            }
        }
    }
    free(file);

    FILE* temp = fopen(newfile, "w");
    fclose(temp);
    editList(newfile);
    return;
   
}

void addDir(char* parentDir){

    char newDir[PATH_MAX]={0};
    strcpy(newDir, trim_whitespaces(parentDir));

    char* Dir = getString("Name new directory", 31, NULL);
    if (Dir == NULL) return;
    if(is_all_space(Dir)){
        free(Dir);
        return;
    }
    strcat(newDir, trim_whitespaces(Dir));
    free(Dir);

    if (mkdir(newDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1){ // "mkdir configDIR"
        printf("error occurred while creating directory.");
        exit(-1);
    }
    return;
}
void editList(char ListName[]){


    //get file path 
    char ListPath[PATH_MAX];
    if(ListName[0] == '/' || ListName[0] == '~'){
        strncpy(ListPath, ListName, PATH_MAX);
    }
    else{
        strcpy(ListPath, config.flashcard_dir);
        strncat(ListPath, ListName, PATH_MAX-strnlen(config.flashcard_dir, 128));
    }
    FlashcardSet* flashcardset=create_Flashcard_Set_Object();
    struct EditorMetadata metadata= {ListPath, flashcardset};
    

    //populate list 
    if (-1 == fillFlashcardSet(metadata.flashcardset, ListPath)) {
        deleteSetPointer(&metadata.flashcardset);
        return;
    }
    _editList(flashcardset, metadata);
}
