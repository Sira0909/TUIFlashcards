#include <linux/limits.h>
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
#include <wchar.h>

#define _XOPEN_SOURCE 600
#include <ncurses.h>
#include <form.h>
#include <config.h>

#include <windows/menu.h>
#include <windows/window.h>

#include <UI.h>
#include <flashcards.h>

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


char selectionkeybinds[11][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select list"},
    {" ", " "},
    {"a", "add list"},
    {"d", "delete list"},
    {"f", "create folder"},
    {" ", " "},
    {"?", "list keybinds"}
};

struct GetListMenuMetadata {
    char* directory;
    int numdirs;
    char (*files)[128];// includes files and directories. first [numdirs] are directories, after are files
    void (*call)(char*);
    char* pickedList;
};
#define Metadata ((struct GetListMenuMetadata*)(((MENU*)menu)->metadata))

static char* _getLists(int start_at, char* directory, void (*to_call)(char*));
static int getLists_keybinds(void* menu);
static int getLists_quit(void* menu);
static int getLists_delete(void* menu);
static int getLists_addlist(void* menu);
static int getLists_createfolder(void* menu);
static int getLists_select(void* menu);

char* _getLists(int start_at, char* dir, void (*to_call)(char*)){

    DIR *dp;

    struct dirent *entry;
    struct stat statbuf;

    //get all lists
    int numfiles = 0;
    int numdirs = 0;
    if((dp = opendir(dir)) == NULL) {
        mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if((dp = opendir(dir)) == NULL) {
            int error = errno;
            endwin();
            printf("ERROR: Could not open or create flashcard directory, which is %s. error description:%s", dir, strerror(error));
            exit(-1);
        }
    }
    chdir(dir);

    while((entry = readdir(dp)) !=NULL){
        lstat(entry->d_name, &statbuf);
        if(entry->d_name[0]!='.'){
            if(!S_ISDIR(statbuf.st_mode)){
                numfiles++;
            }
            else{
                numdirs++;
            }
        }
    }
    closedir(dp);
    //make sure at least one. if not, prompt to create
    if(numfiles == 0){
        addList(dir);
        return NULL;
    }
    else{
        char (*entries)[128] = calloc(numdirs+numfiles, sizeof(char[128]));
        char (*highlight) = calloc(numdirs+numfiles, sizeof(char));
        
        if((dp = opendir(dir)) == NULL) {
            //just in case this suddenly doesnt work
            mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if((dp = opendir(dir)) == NULL) {
                int error = errno;
                endwin();
                printf("ERROR: Could not open or create flashcard directory. error description:%s", strerror(error));
                exit(-1);
            }
        }
        chdir(dir);

        // get all the file paths
        int i = 0;
        int j = 0;
        while((entry = readdir(dp)) != NULL){
            lstat(entry->d_name, &statbuf);
            if(entry->d_name[0]!='.'){
                if(!S_ISDIR(statbuf.st_mode)){
                    if(i<numfiles){
                        strncpy(entries[numdirs+i], entry->d_name,127);
                        i++;
                    }
                }
                else{
                    if(j < numdirs){
                        strncpy(entries[j], entry->d_name,127);
                        highlight[j]='*';
                        j++;
                    }
                }
            }
        }

        closedir(dp);

        // now select between them



        MENU selectmenu;


        // create window for menu. this menu object is defined globally, see above
        WINDOW* select_menu_window = create_newwin(LINES-5, 34, 3, (COLS - 32)/2);

        // init the menu
        init_Menu(&selectmenu, numfiles+numdirs, 32,LINES-8, &select_menu_window, "select list", highlight, entries);
        struct GetListMenuMetadata metadata= {dir, numdirs, entries, to_call, NULL};
        selectmenu.metadata = &metadata;
        wrefresh(selectmenu.window);

        selectmenu.selected = start_at;
        if (selectmenu.selected >= numfiles+numdirs) selectmenu.selected = numfiles+numdirs-1;
        
        addHook_Menu(&selectmenu, (struct hook){'j', &menu_down});
        addHook_Menu(&selectmenu, (struct hook){'k', &menu_up});
        addHook_Menu(&selectmenu, (struct hook){'q', &getLists_quit});
        addHook_Menu(&selectmenu, (struct hook){27,  &getLists_quit});
        addHook_Menu(&selectmenu, (struct hook){'d', &getLists_delete});
        addHook_Menu(&selectmenu, (struct hook){'a', &getLists_addlist});
        addHook_Menu(&selectmenu, (struct hook){'f', &getLists_createfolder});
        addHook_Menu(&selectmenu, (struct hook){10, &getLists_select});
        addHook_Menu(&selectmenu, (struct hook){'?', &getLists_keybinds});
        run_Menu(&selectmenu);
        free(selectmenu.hooks);
        free(highlight);

        return metadata.pickedList;
    }
    return NULL;

}
int getLists_keybinds(void* menu){
    list_keybinds(11, selectionkeybinds);                       return 1;
}

int getLists_quit(void* menu){
        free(Metadata->files);
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
            char path[PATH_MAX];
            strcpy(path,Metadata->directory);
            strcat(path, "/");
            strcat(path,Metadata->files[((MENU*)menu)->selected]);
            int status = remove(path);
            if(status == 0){

                wattron(((MENU*)menu)->window, A_BOLD);
                mvwprintw(((MENU*)menu)->window, LINES-7, 1, "File deleted.");
                wattroff(((MENU*)menu)->window, A_BOLD);



                getLists_quit(menu);
                Metadata->pickedList = _getLists(((MENU*)menu)->selected,Metadata->directory,Metadata->call);
                return -1;
            }
            else{
                if(((MENU*)menu)->selected<Metadata->numdirs){
                    mvwprintw(((MENU*)menu)->window, LINES-7, 1, "                                ");
                    mvwprintw(((MENU*)menu)->window, LINES-7, 1, "Failed to delete. Is it empty?");
                }
                else{
                    wattron(((MENU*)menu)->window, A_BOLD);
                    mvwprintw(((MENU*)menu)->window, LINES-7, 1, "                                ");
                    mvwprintw(((MENU*)menu)->window, LINES-7, 1, "Failed to delete file.");
                    mvwprintw(((MENU*)menu)->window, LINES-7, 1, "Failed to delete.(%s)",strerror(status));
                    wattroff(((MENU*)menu)->window, A_BOLD);
                }
            }
        }
        wrefresh(((MENU*)menu)->window);
        return 1;   
}
int getLists_addlist(void* menu){
    //addlist
    addList(Metadata->directory); 
    //technically quit
    getLists_quit(menu); 
    //rerun with new list. there might be a better way to do this.
    Metadata->pickedList = _getLists(((MENU*)menu)->selected,Metadata->directory,Metadata->call);
    return -1;
}
int getLists_createfolder(void* menu){
    addDir(Metadata->directory);
    getLists_quit(menu); 
    //rerun with new list. there might be a better way to do this.
    Metadata->pickedList = _getLists(((MENU*)menu)->selected,Metadata->directory,Metadata->call);
    return -1;
}
int getLists_select(void* menu){
        if(((MENU*)menu)->selected<Metadata->numdirs){
            char dir[PATH_MAX];
            strncpy(dir, Metadata->directory, PATH_MAX);
            strcat(dir, "/");
            strcat(dir, Metadata->files[((MENU*)menu)->selected]);
            
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(8));
            werase(((MENU*)menu)->window);
            wrefresh(((MENU*)menu)->window);
            Metadata->pickedList = _getLists(0, dir,Metadata->call);
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(2));

            if (Metadata->pickedList!=NULL){
                getLists_quit(menu); return -1;
            }
            else{
                chdir(Metadata->directory);
            }
            return 1;
        }
        else if (Metadata->call == NULL){ 
            Metadata->pickedList = Metadata->files[((MENU*)menu)->selected]; 
            getLists_quit(menu); return -1;
        }
        else{
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(8));
            werase(((MENU*)menu)->window);
            wrefresh(((MENU*)menu)->window);
            char list[PATH_MAX];
            strncpy(list,Metadata->directory, PATH_MAX-128);
            strcat(list,"/");
            strncat(list,Metadata->files[((MENU*)menu)->selected], 128);
            if(strcmp(list+strlen(list)-5, ".list")){
                if(updateList(list)==1){
                    remove(list);
                }
                else{
                    return -1;
                }
                strcat(list, ".list");
            }
            Metadata->call(list);
            wbkgd(((MENU*)menu)->window, COLOR_PAIR(2));
            return 1;
        }
}
#undef Metadata 

char* getLists(void (*to_call)(char*)) {
    return _getLists(0, config.flashcard_dir,to_call);
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

void showmsg(char* msg){
    int mlen = strnlen(msg, 128);
    int width = mlen+4;
    WINDOW* msgWindow = newwin(3, width,(LINES-1)/2, (COLS-(width-2))/2);
    wbkgd(msgWindow, COLOR_PAIR(2));
    box(msgWindow,0,0);
    wprintctrx(msgWindow,1, width,msg);
    wrefresh(msgWindow);
    getch();
    erasewindow(msgWindow);
    return;

}
int getConfirmation(char *question, char* successmsg, char* failmsg){
    int qlen = strnlen(question, 128);
    int width = max(qlen, 8)+4;
    WINDOW* confirmWindow = newwin(7, width,(LINES-5)/2, (COLS-(width-2))/2);
    wbkgd(confirmWindow, COLOR_PAIR(2));
    box(confirmWindow,0,0);
    

    wprintctrx(confirmWindow,2, width,question);
    int ctr = (width)/2;
    mvwprintw(confirmWindow,4,ctr-4, "yes");
    mvwprintw(confirmWindow,4,ctr+1, "no");
    mvwchgat(confirmWindow,4,ctr-4,3,A_BOLD, 3,NULL);
    mvwchgat(confirmWindow,4,ctr+1,2,A_NORMAL, 2,NULL);
    int ch=0;
    bool selection = true;
    while (ch!= 10){
        wrefresh(confirmWindow);
        ch = getch();
        switch(ch){
            case 'j':
            case 'l':
                selection= false;
                mvwchgat(confirmWindow,4,ctr+1,2,A_BOLD, 3,NULL);
                mvwchgat(confirmWindow,4,ctr-4,3,A_NORMAL, 2,NULL);
                break;
            case 'k':
            case 'h':
                selection = true;
                mvwchgat(confirmWindow,4,ctr-4,3,A_BOLD, 3,NULL);
                mvwchgat(confirmWindow,4,ctr+1,2,A_NORMAL, 2,NULL);
                break;
        }
    }
    if(selection&&successmsg!=NULL){
        showmsg(successmsg);
    }else if (!selection&&failmsg!=NULL){
        showmsg(failmsg);
    }
    erasewindow(confirmWindow);
    return selection;
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
                    wasJustBacktick = false;
                    wasJustTilde = false;
                    break;

            case KEY_RIGHT:
                    form_driver(Form, REQ_NEXT_CHAR);
                    wasJustBacktick = false;
                    wasJustTilde = false;
                    break;

            // delete before cursor
            case KEY_BACKSPACE:
            case 127:
                form_driver(Form, REQ_DEL_PREV);
                wasJustBacktick = false;
                wasJustTilde = false;
                break;

            // delete under cursor
            case KEY_DC:
                form_driver(Form, REQ_DEL_CHAR);
                wasJustBacktick = false;
                wasJustTilde = false;
                break;

            handleAccents(Form)
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



void shufflePreserveGraphemes(char* string){
    wchar_t splitgraphemes[strlen(string)];

    int j=0;
    for(int i = 0 ; i <strlen(string);i++){
        if(string[i] == 195-256){
            splitgraphemes[j] = 0xC300+string[i+1];
            i++;
        }
        else
            splitgraphemes[j] = string[i];
        j++;
    }
    for(int i = 0; i < j; i++){
        int ind = rand()%j;
        wchar_t buf = splitgraphemes[ind];
        if(splitgraphemes[ind]!=' ' && splitgraphemes[i]!=' '){
        splitgraphemes[ind]=splitgraphemes[i];
        splitgraphemes[i]=buf;
        }
    }
    for(int i = strlen(string)-1 ; i >=0;i--){
        j--;
        if(splitgraphemes[j]>0xFF){
            string[i]=splitgraphemes[j]%0x100;
            string[i-1] = -61;
            i--;
        }
        else
            string[i] = splitgraphemes[j];
    }
}
