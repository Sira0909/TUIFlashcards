#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define _XOPEN_SOURCE 600
#include <ncurses.h>
#include <config.h>

#include <windows/menu.h>
#include <windows/window.h>

#include <UI.h>
#include <flashcards.h>

char *selectionkeybinds[11][2] = {
    {config.keylayout.str_lkey, "left"},
    {config.keylayout.str_dkey,"down"},
    {config.keylayout.str_ukey,"up"},
    {config.keylayout.str_rkey,"right"},
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
        makedir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
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
            makedir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
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
        
        addHook_Menu(&selectmenu, (struct hook){config.keylayout.dkey, &menu_down});
        addHook_Menu(&selectmenu, (struct hook){config.keylayout.ukey, &menu_up});
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
