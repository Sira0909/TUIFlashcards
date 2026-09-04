#include <stdlib.h>

#include <flashcards.h>
#include <study.h>
#include <string.h>
#include <config.h>
#include <ncurses.h>
#include <windows/menu.h>
#include <windows/table.h>
#include <windows/window.h>

#include <UI.h>
//void (*games[2][3])(FlashcardSet*) ={flashcard,  type};
//
//

struct modeMetadata {
    int selectedx;
    int selectedy;
    WINDOW* mainPlayWindow;
    WINDOW *games[2][3];
    FlashcardSet* flashcard_set;
    char ListPath[PATH_MAX];
};
#define Metadata ((struct modeMetadata*)(metadata))

int render_Mode(void* metadata);
struct modeMetadata Mode_setup(char* list);
int fill(struct modeMetadata metadata);
int Mode_down(void* window);
int Mode_up(void* window);
int Mode_left(void* window);
int Mode_right(void* window);
int Mode_select(void* window);



//TODO: add more modes
void pickMode(char* list){
    bind_keys(getModeKeybinds, render_Mode, 10)
        {config.keylayout.dkey, config.keylayout.str_dkey,"down", &Mode_down},
        {config.keylayout.ukey, config.keylayout.str_ukey,"up", &Mode_up},
        {config.keylayout.rkey, config.keylayout.str_rkey,"right", &Mode_right},
        {config.keylayout.lkey, config.keylayout.str_lkey,"left", &Mode_left},
        {27, " ", " ", &quit},
        {10, "<enter>", "toggle", &Mode_select},
        {'q',"q / <esc>", "quit", &quit},
        {-1, " ", " ", NULL},
        {-1, "?", "list keybinds", NULL},
    };
    struct modeMetadata metadata =Mode_setup(list);
    if (-1 == fill(metadata)) {
        deleteSetPointer(&metadata.flashcard_set);
        return ;
    }


    run(&metadata, getModeKeybinds);
    erasewindow(metadata.mainPlayWindow);
    deleteSetPointer(&metadata.flashcard_set);
}






int render_Mode(void* metadata){
        //correct which is selected
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 3; j++){
                if(i == Metadata->selectedy && j == Metadata->selectedx){
                    wbkgd(Metadata->games[i][j], COLOR_PAIR(3));
                } else
                    wbkgd(Metadata->games[i][j], COLOR_PAIR(2));
                wrefresh(Metadata->games[i][j]);
            }
        }
        
        wrefresh(Metadata->mainPlayWindow);
        return 1;
}
struct modeMetadata Mode_setup(char* list){

    WINDOW* mainPlayWindow = create_newwin(20, 75, (LINES-23)/2, (COLS-74)/2);
    wbkgd(mainPlayWindow, COLOR_PAIR(2));
    box(mainPlayWindow, 0, 0);

    WINDOW* topleft = derwin(mainPlayWindow, 9, 23, 1, 2);
    WINDOW* top = derwin(mainPlayWindow, 9, 23, 1, 26);
    WINDOW* topright = derwin(mainPlayWindow, 9, 23, 1, 50);
    WINDOW* bottomleft = derwin(mainPlayWindow, 9, 23, 10, 2);
    WINDOW* bottom = derwin(mainPlayWindow, 9, 23, 10, 26);
    WINDOW* bottomright = derwin(mainPlayWindow, 9, 23, 10, 50);

    FlashcardSet* flashcard_set = create_Flashcard_Set_Object();
    struct modeMetadata ret =  (struct modeMetadata) {0, 0, mainPlayWindow, { {topleft, top, topright}, {bottomleft, bottom, bottomright} }, flashcard_set, {}};

    wbkgd(topleft, COLOR_PAIR(3));
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 3; j++){
            box(ret.games[i][j], 0, 0);
            wattron(ret.games[i][j], A_BOLD);
        }
    }


    mvwprintw(topleft, 4, (23- 10)/2, "Flashcards" );
    mvwprintw(top, 4, (23 - 15)/2, "Multiple Choice" );
    mvwprintw(topright, 4, (23 - 4)/2, "Type" );


    
    

    if(list[0] == '/' || list[0] == '~' || ( (list[0]& ~32) == 'C' && list[1] == ':' && (list[2] == '/' || list[2]=='\\'))){
        strncpy(ret.ListPath, list, PATH_MAX);
    }
    else{
        strcpy(ret.ListPath, config.flashcard_dir);
        strncat(ret.ListPath, list, PATH_MAX-strnlen(config.flashcard_dir, 128));
    }


    wattron(topleft, A_BOLD);
    wrefresh(mainPlayWindow);
    
    return ret;

}
int fill(struct modeMetadata metadata){
    return fillFlashcardSet(metadata.flashcard_set, metadata.ListPath);
}


int Mode_down(void* metadata){
    
    Metadata->selectedy = (Metadata->selectedy - 1);
    if (Metadata->selectedy < 0) Metadata->selectedy = 1;
    return 1;
}
int Mode_up(void* metadata){
    Metadata->selectedy = (Metadata->selectedy + 1);
    if (Metadata->selectedy > 1) Metadata->selectedy = 0;
    return 1;

}
int Mode_left(void* metadata){
    Metadata->selectedx = (Metadata->selectedx - 1);
    if (Metadata->selectedx < 0) Metadata->selectedx = 2;
    return 1;
}
int Mode_right(void* metadata){
    Metadata->selectedx = (Metadata->selectedx + 1);
    if (Metadata->selectedx > 2) Metadata->selectedx = 0;
    return 1;
}

int Mode_select(void* metadata){
    //hide the menu
    WINDOW* coverWindow = create_newwin(20, 75, (LINES-23)/2, (COLS-74)/2);
    wbkgd(coverWindow, COLOR_PAIR(1));
    wrefresh(coverWindow);
    //run the study meathod
    switch(Metadata->selectedy*3+Metadata->selectedx){
        case 0:
            flashcard(Metadata->flashcard_set);
            break;
        case 1:
            multipleChoice(Metadata->flashcard_set);
            break;
        case 2:
            type(Metadata->flashcard_set);
            break;
        case 3: // unfinished, so hidden but accesible for testing
            test(Metadata->flashcard_set);
            break;
    }
    //save any changes in stars
    writeFlashcardSet(Metadata->flashcard_set, Metadata->ListPath,0);
    //uncover
    erasewindow(coverWindow);
    box(Metadata->mainPlayWindow, 0, 0);
    wrefresh(Metadata->mainPlayWindow);
    return 1;
}










int getOrder(FlashcardSet *flashcard_set, int *(order), bool shuffle, bool starred_only){
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

        mvwaddch(errorWin, 0, 1, ACS_RTEE);
        wprintw(errorWin,"%s", "Error");      
        waddch(errorWin, ACS_LTEE);

        wrefresh(errorWin);
        getch();
        erasewindow(errorWin);
        return 0;
    }
    return numCards;
}
