#include "main.h"
#include <ncurses.h>
#include <time.h>


void flashcard(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){
    srand(time(NULL));

    int order[flashcard_set->num_items];
    int numCards=0;
    for(int i = 0; i<flashcard_set->num_items;i++){
        if(!starred_only || flashcard_set->cards[i].is_starred){
            order[numCards] = i;
            numCards++;
        }
    }
    if(shuffle){
        for(int i = 0; i<numCards; i++){
            int swapindex = rand()%numCards;
            int toswap = order[swapindex];
            order[swapindex] = order[i];
            order[i] = toswap;
        }
    }
    if (numCards == 0){
        WINDOW* errorWin = create_newwin(3, 30, (LINES-1)/2, (COLS-28)/2);
        wbkgd(errorWin, COLOR_PAIR(7));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);
        mvwprintw(errorWin,1,1, "No cards match criteria");
        wmove(errorWin, 0, 1);
        waddch(errorWin, ACS_RTEE);
        wprintw(errorWin, "%s", "Error");
        waddch(errorWin, ACS_LTEE);
        wrefresh(errorWin);
        getch();
        return;
    }
    WINDOW* FlashcardWindow = create_newwin(LINES-5, COLS-5, 2, 2);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    wrefresh(FlashcardWindow);


    int currentcard = 0;
    int ch = -1;
    getch();
    erasewindow(FlashcardWindow);
    
}
