#include <ncurses.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <study.h>
#include <string.h>
#include <config.h>

#include <windows/window.h>
#include <windows/menu.h>

#include <flashcards.h>
#include <UI.h>



char *flashcard_settingskeybinds[10][2] = {
    {config.keylayout.str_dkey,"down"},
    {config.keylayout.str_ukey,"up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {" ", " "},
    {"?", "list keybinds"}
};

struct settingMetadata{
    bool settings_done;
    bool* starred;
    bool* shuffled;
    int* questionCount;
    bool *vectorin;
    bool *vectorout;
    int vectorCount;
};
#define Metadata ((struct settingMetadata*)(((MENU*)menu)->metadata))


int study_settings_keybinds(void* menu){
    list_keybinds(6, flashcard_settingskeybinds);                         return 1;
}

int study_settings_quit(void* menu){
    erasewindow(((MENU*)menu)->window);
    Metadata->settings_done = false;                                      return -1;
}

int study_settings_select(void* menu){
    MENU* settingsmenu = menu;
    char numStr[10]={0};
    switch(settingsmenu->selected){
        case 0:
            *(Metadata->starred) = !(*(Metadata->starred));
            settingsmenu->highlighted[0] = *(Metadata->starred) ? '*' : 0;
            settingsmenu->menuitems[0][19] = *(Metadata->starred)? '*' : ' ';
            break;
        case 1:
            *Metadata->shuffled = !(*Metadata->shuffled);
            settingsmenu->highlighted[1] = *Metadata->shuffled ? '*' : 0;
            settingsmenu->menuitems[1][19] = *Metadata->shuffled ? '*' : ' ';
            break;
        case 3:
            sprintf(numStr, "%d", *(Metadata->questionCount));
            char *newstr = getString("How Many Questions Would You Like?", 10, numStr);
            if(newstr==NULL){
                return 1;
            }
            strncpy(numStr,newstr ,10);
            free(newstr);
            for(int i = 0; i < strnlen(numStr,10);i++){
                if(!isdigit(numStr[i])){
                    showmsg("Invalid Number. Please input a numeric value");
                    return 1;
                }
            }
            sscanf(numStr, "%d", (Metadata->questionCount));
            break;

        case 4:
            wbkgd(wgetparent(settingsmenu->window), COLOR_PAIR(1));
            werase(wgetparent(settingsmenu->window));
            wrefresh(wgetparent(settingsmenu->window));
            editVectors(Metadata->vectorin, Metadata->vectorout, Metadata->vectorCount);
            
            break;
        case 6:
            // clean up
            erasewindow(((MENU*)menu)->window);
            Metadata->settings_done=true;
            return -1;
    }
    return 1;
}


//get seettings for study section
bool get_settings(FlashcardSet *flashcard_set, bool* starred_only, bool* shuffle, int* question_count, bool vectorsin[], bool vectorsout[]){

    struct settingMetadata meta = {false, starred_only,shuffle, question_count,  vectorsin, vectorsout,  flashcard_set->num_columns-1};
    
    MENU setting_menu;

    char items[8][128] = { "Only starred items", "Shuffle flashcards", "", "Edit Question Count", "Edit Study Vectors", "", "Continue\0", "\0"};
    char flags[8] = {0,0,0,1,1,0,0,0};

    // create window for menu. 
    WINDOW* setting_window;
    setting_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    init_Menu(&setting_menu, 7, 20,7, &setting_window, "Settings", flags, items);
    setting_menu.metadata = &meta;
    wrefresh(setting_menu.window);

     
    // character from getch()
    flags[0] = *starred_only ? '*' : 0;
    items[0][19] = *starred_only ? '*' : ' ';
    flags[1] = *shuffle ? '*' : 0;
    items[1][19] = *shuffle ? '*' : ' ';

    addHook_Menu(&setting_menu, (struct hook){config.keylayout.dkey, &menu_down});
    addHook_Menu(&setting_menu, (struct hook){config.keylayout.ukey, &menu_up});
    addHook_Menu(&setting_menu, (struct hook){27, &study_settings_quit});
    addHook_Menu(&setting_menu, (struct hook){'q', &study_settings_quit});
    addHook_Menu(&setting_menu, (struct hook){'?', &study_settings_keybinds});
    addHook_Menu(&setting_menu, (struct hook){10, &study_settings_select});
    run_Menu(&setting_menu);
    free(setting_menu.hooks);
    return meta.settings_done;


}
