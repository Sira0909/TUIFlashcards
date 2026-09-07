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


TABLE editor_setup(FlashcardSet* flashcardset, struct EditorMetadata metadata, WINDOW** edit_list_menu_window){
    int columns = flashcardset->num_columns;

    TABLE flashcardTable;

    int height = min(51, LINES - 5); // this way always big enough
    int width = 23*columns-1;
    *edit_list_menu_window = create_newwin(height+2, width+2, (LINES - height)/2-1, (COLS - width)/2);
    WINDOW* tablewindow = derwin(*edit_list_menu_window, height, width, 1, 1);
    char (**table)[128] = calloc(columns,sizeof(char*));
    char (*starred) = calloc(metadata.flashcardset->capacity, sizeof(char));
    char (*headers)[128] = calloc(columns,sizeof(char[128]));

    


    for(int i = 0 ; i < columns;i++){
        table[i]=calloc(metadata.flashcardset->capacity, sizeof(char[128]));
    }


    getpairslimiter(metadata.flashcardset, starred, table[0], table[1],0);
    for(int i = 1; i < columns-1; i++){
        getDefinitionList(metadata.flashcardset, i, table[i+1]);
        
    }

    

    strcpy(headers[0],"term");
    for(int i = 1; i < columns; i++){
        //TODO: support for definition names
        char str[22];//just in case someone wants 4,000,000,000 definitions for some reason. gotta support that /sarcasm
        sprintf(str, "definition %d", i);
        strcpy(headers[i],str);
    }
    init_Table(&flashcardTable, metadata.flashcardset->num_items, columns,width, height, &tablewindow, "Editing Flashcards", headers, table, starred);

    wbkgd(*edit_list_menu_window, COLOR_PAIR(2));
    box(*edit_list_menu_window, 0, 0);

    wrefresh(*edit_list_menu_window);
    // init the menu
    wrefresh(flashcardTable.window);
    return flashcardTable;

}
void _editList(FlashcardSet* flashcardset, struct EditorMetadata metadata ){

    if (flashcardset->num_columns<2) return;
    
    WINDOW* edit_list_menu_window;
    TABLE flashcardTable = editor_setup(flashcardset, metadata, &edit_list_menu_window);
    flashcardTable.metadata=&metadata;
    

    render_Table(&flashcardTable);
    box(edit_list_menu_window, 0, 0);
    if(flashcardset->num_items>0){
        printProgress(edit_list_menu_window, 0, flashcardset->num_items);
    }
    else{
        printProgress(edit_list_menu_window, -1, 0); // "0/0"
    }

    //add title
    wmove(edit_list_menu_window, 0, 1); waddch(edit_list_menu_window, ACS_RTEE);wprintw(edit_list_menu_window, "%s", "Editing Flashcards"); waddch(edit_list_menu_window, ACS_LTEE);
    wrefresh(edit_list_menu_window);

    bind_keys(editkeybinds, render_Table, 16) 
        {config.keylayout.lkey, config.keylayout.str_lkey, "left",&table_left},
        {config.keylayout.dkey, config.keylayout.str_dkey,"down",&table_down},
        {config.keylayout.ukey, config.keylayout.str_ukey,"up", &table_up},
        {config.keylayout.rkey, config.keylayout.str_rkey,"right", &table_right},
        {10, "<enter>", "edit text", &editor_selectField},
        {config.keylayout.dkey, " ", " ", &editor_update},
        {'s', "s", "star flashcard",&editor_star},
        {'a', "a", "add flashcard", &editor_addCard},
        {'A', "A", "add new definitions",&editor_addDefinition},
        {'d', "d", "delete flashcard",&editor_deleteCard},
        {'D', "D", "remove definitions",&editor_removeDefinition},
        {config.keylayout.ukey, " ", " ", &editor_update},
        {'w', "w", "save", &editor_writeSet},
        {'q', "q", "quit without save", &editor_quit},
        {27, "?", "list keybinds", &editor_quit}
    };


    run(&flashcardTable,editkeybinds);

    for(int i=0; i<flashcardTable.num_cols;i++){
            free(flashcardTable.table_data[i]);
    }
    free(flashcardTable.table_data);
    free(flashcardTable.highlighted);
    free(flashcardTable.headers);
    //cleanup
    erasewindow(flashcardTable.window);
    erasewindow(edit_list_menu_window);
    edit_list_menu_window = NULL;
    flashcardTable.window = NULL;
    refresh();
}
int editor_update(void*table){
    TABLE* Table = (TABLE*)table;
    if(Metadata->flashcardset->num_items>0){
        printProgress(wgetparent(Table->window), Table->selected_row, Metadata->flashcardset->num_items);
    }
    else{
        printProgress(wgetparent(Table->window), -1, 0); // "0/0"
    }
    wrefresh(wgetparent(Table->window));
    return 1;
}

int editor_addDefinition(void*table){
    TABLE* Table = (TABLE*)table;
    wbkgd(wgetparent(Table->window), COLOR_PAIR(1));
    werase(wgetparent(Table->window));
    wrefresh(wgetparent(Table->window));
    if(addDefn(Metadata->flashcardset, NULL)==-1){
        showmsg("an error occured trying to add a definition. Exiting!");
        endwin();
        printf("error: realloc failed in flaschards.c:addDefn()");
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
            showmsg("an error occured trying to remove a definition. Exiting!");
            endwin();
            printf("error: realloc failed in flaschards.c:delDefn()");
            exit(-1);
        }

        _editList(Metadata->flashcardset, *Metadata);
        return -1;
    }
    else{
        showmsg("Cannot delete the term column");
        return 1;
    }
}
int editor_star(void* table){
    if(Metadata->flashcardset->num_items==0){
        showmsg("There is no card to star!");
        return 1;
    }
    TABLE* Table = (TABLE*)table;
    Metadata->CurrentCard.is_starred = !Metadata->CurrentCard.is_starred;
    Table->highlighted[Table->selected_row]=(Metadata->CurrentCard.is_starred)? '*': ' ';
    return 1;
}
int editor_selectField(void* table){
    if(Metadata->flashcardset->num_items==0){
        showmsg("There is no card to edit!");
        return 1;
    }
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
    if(Metadata->flashcardset->num_items==0){
        showmsg("There is no card to delete!");
        return 1;
    }

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
            //TODO: make this give a name for defns
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
                mvwprintw(Table->window, Table->height-1, 0, "quit without saving? (y/n): ");
                wattroff(Table->window, A_BOLD);
                wrefresh(Table->window);
                if ('y' == getch()){
                    //clean up and quit
                    deleteSetPointer(&Metadata->flashcardset);
                    return -1;
                }
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
    
    if(strlen(file)<6||(strcmp(file+strlen(file)-6,".list")&&strlen(file)+5<PATH_MAX)){
        strcat(file, ".list"); 
    }
    strcat(newfile, trim_whitespaces(file));
    if(create==2){
        for(int i = strlen(dir)+1;i<strnlen(newfile,PATH_MAX);i++){
            if(newfile[i]=='/'){
                newfile[i]='\0';
                if(makedir(newfile,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)==-1){showmsg("Failed to create directory");return;}
                newfile[i]='/';
            }
        }
    }
    free(file);

    FlashcardSet* fcs = create_Flashcard_Set_Object();
    writeFlashcardSet(fcs, newfile, 1);
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

    if (makedir(newDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1){ // "mkdir configDIR"
        printf("error occurred while creating directory.");
        exit(-1);
    }
    return;
}
void editList(char ListName[]){


    //get file path 
    char ListPath[PATH_MAX];
    if(ListName[0] == '/' || ListName[0] == '~' || ( (ListName[0]& ~32) == 'C' && ListName[1] == ':' && (ListName[2] == '/' || ListName[2]=='\\'))){
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
