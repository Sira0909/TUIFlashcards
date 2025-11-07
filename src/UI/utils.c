#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <macros.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define _XOPEN_SOURCE 600
#include <ncurses.h>
#include <form.h>
#include <config.h>

#include <windows/menu.h>
#include <windows/window.h>

#include <UI.h>

char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(!strcmp(str," ") || !strcmp(str, "\n"))
		str++;

	if(*str == '\0') // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str 
        &&   (!strcmp(end," ")      // trim space
        ||    !strcmp(end,"\n")))   // trim newline
                    end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

int is_all_space(char *string){
    for (int i = 0; string[i] != '\0'; i++){
        if(string[i]!= ' ') return 0;
    }
    return 1;
}


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

char (*glofiles)[128];
char* toreturn;
void (*call)(char*);
char* _getLists(int start_at, void (*to_call)(char*));


int getLists_keybinds(void* menu){
    list_keybinds(10, selectionkeybinds);                       return 1;
}

int getLists_quit(void* menu){
        free(glofiles);
        erasewindow(((MENU*)menu)->window);
        ((MENU*)menu)->window= NULL;
        return -1;
}
int getLists_delete(void* menu){
        wattron(((MENU*)menu)->window, A_BOLD);
        mvwprintw(((MENU*)menu)->window, LINES-7, 1, "Really delete (needs capital Y)?");
        wattroff(((MENU*)menu)->window, A_BOLD);
        wrefresh(((MENU*)menu)->window);
        if (getch()=='Y'){
            if(remove(glofiles[((MENU*)menu)->selected]) == 0){

                wattron(((MENU*)menu)->window, A_BOLD);
                mvwprintw(((MENU*)menu)->window, LINES-7, 1, "File deleted.");
                wattroff(((MENU*)menu)->window, A_BOLD);



                getLists_quit(menu);
                toreturn = _getLists(((MENU*)menu)->selected,call);
                return -1;
            }
        }
        mvwprintw(((MENU*)menu)->window, LINES-7, 1, "                                ");
        wrefresh(((MENU*)menu)->window);
        return 1;
}
int getLists_addlist(void* menu){
    //addlist
    addList(); 
    //technically quit
    getLists_quit(menu); 
    //rerun with new list. there might be a better way to do this.
    toreturn = _getLists(((MENU*)menu)->selected,call);
    return -1;
}
int getLists_select(void* menu){
        if (call == NULL){ toreturn = glofiles[((MENU*)menu)->selected]; getLists_quit(menu); return -1;}
        else{
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(8));
            werase(((MENU*)menu)->window);
            wrefresh(((MENU*)menu)->window);
            call(glofiles[((MENU*)menu)->selected]);
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(2));
            return 1;
        }
}
char* _getLists(int start_at, void (*to_call)(char*)){

    DIR *dp;

    struct dirent *entry;
    struct stat statbuf;

    //get all lists
    int numfiles = 0;
    if((dp = opendir(config.flashcard_dir)) == NULL) {
        mkdir(config.flashcard_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if((dp = opendir(config.flashcard_dir)) == NULL) {
            int error = errno;
            endwin();
            printf("ERROR: Could not open or create flashcard directory, which is %s. error description:%s", config.flashcard_dir, strerror(error));
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
    closedir(dp);
    //make sure at least one. if not, prompt to create
    if(numfiles == 0){
        addList();
        return NULL;
    }
    else{
        char (*files)[128] = calloc(numfiles, sizeof(char[128]));
        if((dp = opendir(config.flashcard_dir)) == NULL) {
            //just in case this suddenly doesnt work
            mkdir(config.flashcard_dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if((dp = opendir(config.flashcard_dir)) == NULL) {
                int error = errno;
                endwin();
                printf("ERROR: Could not open or create flashcard directory. error description:%s", strerror(error));
                exit(-1);
            }
        }
        chdir(config.flashcard_dir);

        // get all the file paths
        int i = 0;
        while((entry = readdir(dp)) != NULL){
            lstat(entry->d_name, &statbuf);
            if(!S_ISDIR(statbuf.st_mode)){
                strncpy(files[i], entry->d_name,127);
                i++;
            }
        }
        closedir(dp);

        // now select between them



        MENU selectmenu;


        // create window for menu. this menu object is defined globally, see above
        WINDOW* select_menu_window = create_newwin(LINES-5, 34, 3, (COLS - 32)/2);

        // init the menu
        init_Menu(&selectmenu, numfiles, 32,LINES-8, &select_menu_window, "select list", NULL, files);
        wrefresh(selectmenu.window);

        selectmenu.selected = start_at;
        if (selectmenu.selected >= numfiles) selectmenu.selected = numfiles-1;
        
        glofiles = files;
        call = to_call;

        addHook_Menu(&selectmenu, (struct hook){'j', &menu_down});
        addHook_Menu(&selectmenu, (struct hook){'k', &menu_up});
        addHook_Menu(&selectmenu, (struct hook){'q', &getLists_quit});
        addHook_Menu(&selectmenu, (struct hook){27,  &getLists_quit});
        addHook_Menu(&selectmenu, (struct hook){'d', &getLists_delete});
        addHook_Menu(&selectmenu, (struct hook){'a', &getLists_addlist});
        addHook_Menu(&selectmenu, (struct hook){10, &getLists_select});
        addHook_Menu(&selectmenu, (struct hook){'?', &getLists_keybinds});
        run_Menu(&selectmenu);
        free(selectmenu.hooks);

        return toreturn;
    }
    return NULL;

}

char* getLists(void (*to_call)(char*)) {
    return _getLists(0, to_call);
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

char* getString(char* title, int maxsize, char* startingText){
    WINDOW *my_form_win;
    FIELD *FileNameField[2];
    FORM *Form;
    int ch, rows, cols;




    //set up form box

    //create 1 element form
    FileNameField[0] = new_field(1, 30, 0, 0, (maxsize - 1)/30, 0);
    FileNameField[1] = NULL;

    // set field atributes
    set_field_back(FileNameField[0], COLOR_PAIR(3));
    field_opts_off(FileNameField[0], O_AUTOSKIP);
    field_opts_off(FileNameField[0], O_WRAP);

    //create form
    Form = new_form(FileNameField);
    
    //assign window
    scale_form(Form, &rows, &cols);
    my_form_win = newwin(rows+4, cols+4,(LINES - rows-2)/2,(COLS - cols-2)/2);

    // if we have starting text, use it
    if(startingText!=NULL){
        set_field_buffer(FileNameField[0], 0, startingText);
        //form_driver(Form, REQ_END_FIELD);
    }

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

    //for accents
    bool wasJustBacktick = false;
    bool wasJustTilde = false;

    while((ch = getch())){
        switch (ch){
            case 10: //enter
                //do return stuff
                form_driver(Form, REQ_NEXT_FIELD);
                form_driver(Form, REQ_PREV_FIELD);
                char* string = calloc(maxsize+1, sizeof(char));
                string = strncpy(string, field_buffer(FileNameField[0],0), maxsize);
                int end = maxsize-1;
                //trims the whitespace from end of form
                while(string[--end] == '\0' || string[end] == ' '){
                }
                string[end+1] = '\0';
                
                //turn cursor back off
                curs_set(0);

                //cleanup
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                erasewindow(my_form_win);
                refresh();
                return string;
                
            case 27: //cancel (esc key)

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

            case '`':
                // for accents
                if(config.autoaccent>0){
                    wasJustTilde=false;
                    // double backtick should cancel accent
                    if(!wasJustBacktick){
                        wasJustBacktick= true;
                        form_driver(Form, ch);
                    }
                    else{
                        wasJustBacktick= false;
                    }
                    break;
                }
            case '~':
                if(config.autoaccent>0){
                    wasJustBacktick=false;
                    // double tilde should cancel it
                    if(!wasJustTilde){
                        wasJustTilde = true;
                        form_driver(Form, ch);
                    }
                    else{ // double ~
                        wasJustTilde = false;
                    }
                    break;
                }
            default:
                if(config.autoaccent>0){
                    //handle accent marks
                    if(wasJustBacktick){
                        switch(ch){
                            case 'a':
                                ch = L'á';
                                break;
                            case 'e':
                                ch = L'é';
                                break;
                            case 'i':
                                ch = L'í';
                                break;
                            case 'o':
                                ch = L'ó';
                                break;
                            case 'u':
                                ch = L'Ú';
                                break;
                            case 'A':
                                ch = L'Á';
                                break;
                            case 'E':
                                ch = L'É';
                                break;
                            case 'I':
                                ch = L'Í';
                                break;
                            case 'O':
                                ch = L'Ó';
                                break;
                            case 'U':
                                ch = L'Ú';
                                break;
                            default:
                                form_driver(Form, REQ_NEXT_CHAR);
                                break;
                        }
                        form_driver(Form, REQ_DEL_PREV);
                    }
                    else if(wasJustTilde && ch=='n'){
                        ch = L'ñ';
                        form_driver(Form, REQ_DEL_PREV);
                    }
                    else if(wasJustTilde && ch=='N'){
                        ch = L'Ñ';
                        form_driver(Form, REQ_DEL_PREV);
                    }
                }
                form_driver_w(Form, OK, ch);
                wasJustBacktick = false;
                wasJustTilde = false;
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
