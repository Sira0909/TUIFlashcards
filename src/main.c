#include "constants.h"
#include "window.h"
#include "WIN.h"
#include "MENU.h"
#include "flashcards.h"
#include "TABLE.h"
#include "config.h"
#include "play/main.h"
#include "helpers.h"
#include <ncurses.h>

bool is_all_space(char *string){
    for (int i = 0; string[i] != '\0'; i++){
        if(string[i]!= ' ') return false;
    }
    return true;
}


// lists of keybinds
char mainkeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};
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
char selectionkeybinds[10][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select list"},
    {" ", " "},
    {"a", "add list"},
    {"d", "delete list"},
    {" ", " "},
    {"?", "list keybinds"}
};


void runMainMenu();
void editList(char ListName[]);
void addList();

char* getLists(void (*to_call)(char*)) ;
void play(char* list);

void list_keybinds(int numBinds, char (*keybinds)[2][20]);

int ListPick();

WINDOW* menu_window; //main menu window ptr

int main(){

    get_config_struct(&config);


    // init ncurses
    initscr();

    //start color
    start_color();



    if (can_change_color()){
        // colorscheme based on tokyonight
        init_color(COLOR_BLACK, 106, 114, 169);
        init_color(COLOR_RED, 1000, 459,498);
        init_color(COLOR_YELLOW, 1000, 780, 467);
        init_color(COLOR_BLUE, 510, 667, 1000);
        init_color(COLOR_WHITE, 510, 545, 722);

    }

    // colors
    init_pair(1, COLOR_YELLOW , COLOR_BLUE);     // background:          white on blue
    init_pair(2, COLOR_BLACK , COLOR_WHITE);    // default window:      black on white
    init_pair(3, COLOR_BLACK , COLOR_RED);      // selection:           white on red
    init_pair(4, COLOR_YELLOW, COLOR_WHITE);      // selection:           white on red
    init_pair(5, COLOR_YELLOW, COLOR_RED);      // selection:           white on red
    init_pair(6, COLOR_BLUE, COLOR_WHITE);      // selection:           white on red
    init_pair(7, COLOR_BLUE, COLOR_RED);      // selection:           white on red
    init_pair(8, COLOR_WHITE , COLOR_BLUE);     // background:          white on blue

    // set ncurses modes
    cbreak();//change later
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    // set background
    bkgd(COLOR_PAIR(1));
    attron(A_BOLD);
    attron(COLOR_PAIR(5));
    mvprintw(LINES-1, (COLS-16)/2, "'?' for keybinds");
    attroff(COLOR_PAIR(5));
    attroff(A_BOLD);
    refresh();

    //runMainMenu - all things happen from here.
    runMainMenu();

    // clean up
    endwin();
    
    return 0;
}

// main menu function
void runMainMenu(){
    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU mainmenu;

    // list of options
    char items[7][128] = {"Study\0", "\0", "New List\0", "Edit List\0", "\0", "Quit\0", "\0"};

    // create window for menu. this menu object is defined globally, see above
    menu_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    // init the menu
    init_menu(&mainmenu, 7, 20,7, &menu_window, "Let's Study!", items);
    wrefresh(mainmenu.p_win->window);
    
    // character from getch()
    int ch;

    // so that we can leave while loop
    bool done = false;
    while(!done){
        // refresh menu (see MENU.c)
        render_menu(&mainmenu, NULL) ;
        ch = getch();
        switch(ch){
            case 'j': // down
                changeselect(&mainmenu, 1);
                break;
            case 'k': // up
                changeselect(&mainmenu, -1);
                break;
            case 'q': // quit
                done = true;
                break;
            case 10: // enter
                wmove(stdscr, LINES-1, 0);
                wclrtoeol(stdscr);
                wrefresh(stdscr);
                switch(mainmenu.selected){
                    case 0: // "Study"
                        getLists(play);
                        break;
                    case 2: // "New List"
                        addList();
                        break;
                    case 3: // "Edit List"
                        getLists(editList);
                        break;
                    case 5: // "Quit"
                        done = true;
                        break;
                    default:// error, debug info will be printed
                        wprintw(menu_window, "%d", mainmenu.selected);
                        break;
                }
                break;
            case '?':
                list_keybinds(7, mainkeybinds);
                break;
        }
                
    }
    
    // clean up
    mainmenu.p_win->window = NULL;
    free(mainmenu.p_win);
    delwin(menu_window);


}
char* getString(char* title, int maxsize){
    WINDOW *my_form_win;
    FIELD *FileNameField[2];
    FORM *Form;
    int ch, rows, cols;

    //create 1 element form
    FileNameField[0] = new_field(1, 30, 0, 0, maxsize - 31, 0);
    FileNameField[1] = NULL;

    // set field atributes
    set_field_back(FileNameField[0], COLOR_PAIR(3));
    field_opts_off(FileNameField[0], O_AUTOSKIP);

    //create form
    Form = new_form(FileNameField);
    
    //assign window
    scale_form(Form, &rows, &cols);
    my_form_win = newwin(rows+4, cols+4,(LINES - rows-2)/2,(COLS - cols-2)/2);

    set_form_win(Form, my_form_win);
    set_form_sub(Form, derwin(my_form_win, rows, cols, 3, 2));
    
    //set form background
    wbkgd(my_form_win, COLOR_PAIR(2));


    box(my_form_win, 0, 0);
    //wmove(my_form_win, 0, 1); waddch(my_form_win, ACS_RTEE);wprintw(my_form_win, "%s", "double escape to cancel"); waddch(my_form_win, ACS_LTEE);
    mvwprintw(my_form_win, 1, (cols-strlen(title)+1)/2, "%s", title);

    post_form(Form);
    wrefresh(my_form_win);

    curs_set(1);

    int length = 0;
    while((ch = getch())){
        switch (ch){
            case 10: //enter
                //do return stuff
                form_driver(Form, REQ_NEXT_FIELD);
                form_driver(Form, REQ_PREV_FIELD);
                char* string = calloc(maxsize+1, sizeof(char));
                string = strncpy(string, field_buffer(FileNameField[0],0), maxsize);
                int end = maxsize;
                while(string[--end] == '\0' || string[end] == ' '){
                }
                string[end+1] = '\0';
                curs_set(0);
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                erasewindow(my_form_win);
                refresh();
                return string;
                
            case 27:

                curs_set(0);
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                wborder(my_form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
                wbkgd(my_form_win, COLOR_PAIR(1));
                wrefresh(my_form_win);
                delwin(my_form_win);
                refresh();
                return NULL;

            //move cursor
            case KEY_LEFT:
                    form_driver(Form, REQ_PREV_CHAR);
                    break;

            case KEY_RIGHT:
                    form_driver(Form, REQ_NEXT_CHAR);
                    break;

            // delete before cursor
            case KEY_BACKSPACE:
            case 127:
                form_driver(Form, REQ_DEL_PREV);
                break;

            // delete under cursor
            case KEY_DC:
                form_driver(Form, REQ_DEL_CHAR);
                break;


            default:
                form_driver(Form, ch);
                break;
        }
        wrefresh(my_form_win);
    }
    curs_set(0);

    unpost_form(Form);
    free_form(Form);
    free_field(FileNameField[0]);
    wborder(my_form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
    wbkgd(my_form_win, COLOR_PAIR(1));
    wrefresh(my_form_win);
    delwin(my_form_win);
    refresh();

    return NULL;
}
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
    wrefresh(flashcardTable.p_win->window);
    
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
                        erasewindow(flashcardTable.p_win->window);
                        erasewindow(edit_list_menu_window);
                        edit_list_menu_window = NULL;
                        flashcardTable.p_win->window = NULL;
                        free(flashcardTable.p_win);
                        flashcardTable.p_win = NULL;
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
    erasewindow(flashcardTable.p_win->window);
    erasewindow(edit_list_menu_window);
    edit_list_menu_window = NULL;
    flashcardTable.p_win->window = NULL;
    free(flashcardTable.p_win);
    flashcardTable.p_win = NULL;
    free(items);
    free(defns);
    deleteSetPointer(&flashcardset);
    refresh();
}


char* _getLists(int start_at, void (*to_call)(char*)){

    DIR *dp;

    struct dirent *entry;
    struct stat statbuf;

    int numfiles = 0;
    if((dp = opendir(config.flashcard_dir)) == NULL) {
        mkdir(config.flashcard_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if((dp = opendir(config.flashcard_dir)) == NULL) {
            int error = errno;
            endwin();
            printf("ERROR: Could not open or create flashcard directory. error description:%s", strerror(error));
            exit(-1);
        }
    }
    chdir(config.flashcard_dir);

    while((entry = readdir(dp)) !=NULL){
        lstat(entry->d_name, &statbuf);
        if(!S_ISDIR(statbuf.st_mode)){
            numfiles++;
        }
    }
    if(numfiles == 0){
        addList();
        return NULL;
    }
    else{
        char (*files)[128] = calloc(numfiles, sizeof(char[128]));
        if((dp = opendir(config.flashcard_dir)) == NULL) {
            mkdir(config.flashcard_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if((dp = opendir(config.flashcard_dir)) == NULL) {
                int error = errno;
                endwin();
                printf("ERROR: Could not open or create flashcard directory. error description:%s", strerror(error));
                exit(-1);
            }
        }
        chdir(config.flashcard_dir);

        int i = 0;
        while((entry = readdir(dp)) != NULL){
            lstat(entry->d_name, &statbuf);
            if(!S_ISDIR(statbuf.st_mode)){
                strncpy(files[i], entry->d_name,127);
                i++;
            }
        }
        MENU selectmenu;


        // create window for menu. this menu object is defined globally, see above
        WINDOW* select_menu_window = create_newwin(LINES-5, 34, 3, (COLS - 32)/2);

        // init the menu
        init_menu(&selectmenu, numfiles, 32,LINES-8, &select_menu_window, "select list", files);
        wrefresh(selectmenu.p_win->window);

        selectmenu.selected = start_at;
        if (selectmenu.selected >= numfiles) selectmenu.selected = numfiles-1;
        
        // character from getch()
        int ch;

        // so that we can leave while loop
        bool done = false;
        while(!done){
            // refresh menu (see MENU.c)
            render_menu(&selectmenu,NULL) ;
            ch = getch();
            switch(ch){
                case 'j': // down
                    changeselect(&selectmenu, 1);
                    break;
                case 'k': // up
                    changeselect(&selectmenu, -1);
                    break;
                case 'q': // quit
                    done = true;
                    break;
                case 'd':
                    wattron(select_menu_window, A_BOLD);
                    mvwprintw(select_menu_window, LINES-7, 1, "Really delete (needs capital Y)?");
                    wattroff(select_menu_window, A_BOLD);
                    wrefresh(select_menu_window);
                    if (getch()=='Y'){
                        if(remove(files[selectmenu.selected]) == 0){
                            wattron(select_menu_window, A_BOLD);
                            mvwprintw(select_menu_window, LINES-7, 1, "File deleted.");
                            wattroff(select_menu_window, A_BOLD);
                            selectmenu.p_win->window = NULL;
                            free(selectmenu.p_win);
                            free(files);
                            erasewindow(select_menu_window);
                            return _getLists(selectmenu.selected,to_call);
                        }
                    }
                    mvwprintw(select_menu_window, LINES-7, 1, "                                ");
                    wrefresh(select_menu_window);

                    break;
                case 'a':
                    addList();
                    selectmenu.p_win->window = NULL;
                    free(selectmenu.p_win);
                    free(files);
                    erasewindow(select_menu_window);
                    return _getLists(selectmenu.selected, to_call);
                case 10: // enter
                    if (to_call == NULL){
                        return files[selectmenu.selected];
                    }
                    else{
                        wbkgd(select_menu_window, COLOR_PAIR(8));
                        werase(select_menu_window);
                        wrefresh(select_menu_window);
                        to_call(files[selectmenu.selected]);
                        wbkgd(select_menu_window, COLOR_PAIR(2));
                    }
                    break;
                case '?':
                    list_keybinds(10, selectionkeybinds);
            }
                    
        }
        selectmenu.p_win->window = NULL;
        free(selectmenu.p_win);
        free(files);
        erasewindow(select_menu_window);
    }
    return NULL;

}
char* getLists(void (*to_call)(char*)) {
    return _getLists(0, to_call);
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



void list_keybinds(int numBinds, char (*keybinds)[2][20]){
    WINDOW* helpwindow = create_newwin(numBinds+4, 32, (LINES-numBinds-2)/2, COLS/2 - 15);
    wbkgd(helpwindow, COLOR_PAIR(2));
    wattron(helpwindow, A_BOLD);

    box(helpwindow, 0, 0);


    mvwprintw(helpwindow, 1, 1, "keybinds:");

    for(int i = 0; i<numBinds; i++){
        mvwprintw(helpwindow, 3+i, 1, "%s", keybinds[i][0]);
        
        mvwprintw(helpwindow, 3+i, 31-strnlen(keybinds[i][1],20), "%s", keybinds[i][1]);
    }

    wrefresh(helpwindow);
    getch();
    erasewindow(helpwindow);

}
