#include <study.h>
#include <string.h>
#include <stdlib.h>
#include <config.h>
#include <ncurses.h>
#include <time.h>
#include <windows/menu.h>
#include <windows/window.h>

#include <UI.h>
//void (*games[2][3])(FlashcardSet*) ={flashcard,  type};
//
//
char settingskeybinds[10][2][20] = {
    {"j","down"},
    {"k","up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {" ", " "},
    {"?", "list keybinds"}
};

char playkeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};

int getOrder(FlashcardSet *flashcard_set, int *(order), bool shuffle, bool starred_only){
    srand(time(NULL));
    int numCards=0;
    // filter out unstarred if only starred
    for(int i = 0; i<flashcard_set->num_items;i++){
        if(!starred_only || flashcard_set->cards[i].is_starred){
            order[numCards] = i;
            numCards++;
        }
    }

    //shuffle
    if(shuffle){
        for(int i = 0; i<numCards; i++){
            int swapindex = rand()%numCards;
            int toswap = order[swapindex];
            order[swapindex] = order[i];
            order[i] = toswap;
        }
    }
    // error if no possible cards
    if (numCards == 0){
        WINDOW* errorWin = create_newwin(3, 30, (LINES-1)/2, (COLS-28)/2);
        wbkgd(errorWin, COLOR_PAIR(7));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);

        mvwprintw(errorWin,1,1, "No cards match criteria");
        mvwprintw(errorWin,0,1, "%c%s%c", ACS_RTEE, "Error", ACS_LTEE);      

        wrefresh(errorWin);
        getch();
        erasewindow(errorWin);
        return 0;
    }
    return numCards;
}


bool settings_done;
bool* starred;
bool* shuffled;
int settings_keybinds(void* menu){
    list_keybinds(6, settingskeybinds);                         return 1;
}

int settings_quit(void* menu){
    erasewindow(((MENU*)menu)->window);
    settings_done = false;                                      return -1;
}

int settings_select(void* menu){
    MENU* settingsmenu = menu;
    switch(settingsmenu->selected){
        case 0:
            *starred = !(*starred);
            settingsmenu->highlighted[0] = *starred ? '*' : 0;
            settingsmenu->menuitems[0][19] = *starred ? '*' : ' ';
            break;
        case 1:
            *shuffled = !(*shuffled);
            settingsmenu->highlighted[1] = *shuffled ? '*' : 0;
            settingsmenu->menuitems[1][19] = *shuffled ? '*' : ' ';
            break;
        case 3:
            // clean up
            erasewindow(((MENU*)menu)->window);
            settings_done=true;
            return -1;
    }
    return 1;
}


//get seettings for study section
bool get_settings(bool* starred_only, bool* shuffle){
    starred = starred_only;
    shuffled = shuffle;
    MENU setting_menu;

    char items[5][128] = { "Only starred items", "Shuffle flashcards", "\0", "Continue\0", "\0"};
    char flags[5] = {0,0,0,0,0};

    // create window for menu. 
    WINDOW* setting_window;
    setting_window = create_newwin(7, 23, (LINES - 5)/2, (COLS - 20)/2);

    init_Menu(&setting_menu, 5, 20,5, &setting_window, "Settings", flags, items);
    wrefresh(setting_menu.window);

     
    // character from getch()
    flags[0] = *starred_only ? '*' : 0;
    items[0][19] = *starred_only ? '*' : ' ';
    flags[1] = *shuffle ? '*' : 0;
    items[1][19] = *shuffle ? '*' : ' ';

    addHook_Menu(&setting_menu, (struct hook){'j', &menu_down});
    addHook_Menu(&setting_menu, (struct hook){'k', &menu_up});
    addHook_Menu(&setting_menu, (struct hook){27, &settings_quit});
    addHook_Menu(&setting_menu, (struct hook){'q', &settings_quit});
    addHook_Menu(&setting_menu, (struct hook){'?', &settings_keybinds});
    addHook_Menu(&setting_menu, (struct hook){10, &settings_select});
    run_Menu(&setting_menu);
    free(setting_menu.hooks);
    return settings_done;


}

//TODO: add more modes
void pickMode(char* list){
    WINDOW* mainPlayWindow = create_newwin(20, 75, (LINES-23)/2, (COLS-74)/2);
    wbkgd(mainPlayWindow, COLOR_PAIR(2));
    box(mainPlayWindow, 0, 0);

    WINDOW* topleft = derwin(mainPlayWindow, 9, 23, 1, 2);
    WINDOW* top = derwin(mainPlayWindow, 9, 23, 1, 26);
    WINDOW* topright = derwin(mainPlayWindow, 9, 23, 1, 50);
    WINDOW* bottomleft = derwin(mainPlayWindow, 9, 23, 10, 2);
    WINDOW* bottom = derwin(mainPlayWindow, 9, 23, 10, 26);
    WINDOW* bottomright = derwin(mainPlayWindow, 9, 23, 10, 50);

    wbkgd(topleft, COLOR_PAIR(3));

    box(topleft, 0, 0);
    box(top, 0, 0);
    box(topright, 0, 0);
    box(bottomleft, 0, 0);
    box(bottom, 0, 0);
    box(bottomright, 0, 0);

    wattron(topleft, A_BOLD);
    wattron(top, A_BOLD);
    wattron(topright, A_BOLD);
    wattron(bottomleft, A_BOLD);
    wattron(bottom, A_BOLD);
    wattron(bottomright, A_BOLD);

    mvwprintw(topleft, 4, (23- 10)/2, "Flashcards" );
    mvwprintw(top, 4, (23 - 15)/2, "Multiple Choice" );
    mvwprintw(topright, 4, (23 - 4)/2, "Type" );


    char ListPath[PATH_MAX];
    if(list[0] == '/' || list[0] == '~'){
        strncpy(ListPath, list, PATH_MAX);
    }
    else{
        strcpy(ListPath, config.flashcard_dir);
        strncat(ListPath, list, PATH_MAX-strnlen(config.flashcard_dir, 128));
    }
    
    FlashcardSet* flashcard_set = create_Flashcard_Set_Object();
    
    if (-1 == fillFlashcardSet(flashcard_set, ListPath)) {
        deleteSetPointer(&flashcard_set);
        return;
    }


    wattron(topleft, A_BOLD);
    wrefresh(mainPlayWindow);

    int selectedx = 0;
    int selectedy = 0;

    WINDOW *games[2][3] = { {topleft, top, topright}, {bottomleft, bottom, bottomright} };

    int ch;
    while ((ch = getch())){
        
        switch(ch){
            case 'h': 
                selectedx = (selectedx - 1);
                if (selectedx < 0) selectedx = 2;
                break;
            case 'j':
                selectedy = (selectedy - 1) % 3;
                if (selectedy < 0) selectedy = 1;
                break;
            case 'k':
                selectedy = (selectedy + 1) % 3;
                if (selectedy > 1) selectedy = 0;
                break;
            case 'l':
                selectedx = (selectedx + 1) % 3;
                if (selectedx > 2) selectedx = 0;
                break;
            case 27:
            case 'q':
                erasewindow(mainPlayWindow);
                deleteSetPointer(&flashcard_set);
                return;
            case 10:
                {

                    //hide the menu
                    WINDOW* coverWindow = create_newwin(20, 75, (LINES-23)/2, (COLS-74)/2);
                    wbkgd(coverWindow, COLOR_PAIR(1));
                    wrefresh(coverWindow);
                    //run the study meathod
                    switch(selectedy*3+selectedx){
                        case 0:
                            flashcard(flashcard_set);
                            break;
                        case 1:
                            multipleChoice(flashcard_set);
                            break;
                        case 2:
                            type(flashcard_set);
                            break;
                    }
                    //save any changes in stars
                    writeFlashcardSet(flashcard_set, ListPath,0);
                    //uncover
                    erasewindow(coverWindow);
                    box(mainPlayWindow, 0, 0);
                    wrefresh(mainPlayWindow);
                    break;
                }
            case '?':
                list_keybinds(7, settingskeybinds);
                box(mainPlayWindow, 0, 0);
                wrefresh(mainPlayWindow);
                break;
        }
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 3; j++){
                if(i == selectedy && j == selectedx){
                    wbkgd(games[i][j], COLOR_PAIR(3));
                } else
                    wbkgd(games[i][j], COLOR_PAIR(2));
                wrefresh(games[i][j]);
            }
        }
        
        wrefresh(mainPlayWindow);
    }
    getch();
    erasewindow(mainPlayWindow);
    deleteSetPointer(&flashcard_set);
}
