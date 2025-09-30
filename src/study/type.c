#include <study/modes.h>
#include <stdlib.h>
#include <windows/window.h>
#define complete 0
void type(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){

    int order[flashcard_set->num_items];
    int numCards=0;
    int maxlength =25;
    for(int i = 0; i<flashcard_set->num_items;i++){
        if(!starred_only || flashcard_set->cards[i].is_starred){
            if (strlen(flashcard_set->cards[i].definition)>maxlength){
                maxlength = strlen(flashcard_set->cards[i].definition);
            }
            if (strlen(flashcard_set->cards[i].name)>maxlength){
                maxlength = strlen(flashcard_set->cards[i].name);
            }
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
        erasewindow(errorWin);
        return;
    }


    if(!complete){
        WINDOW* errorWin = create_newwin(5, 46, (LINES-8)/2, (COLS-44)/2);
        wbkgd(errorWin, COLOR_PAIR(2));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);
        mvwprintw(errorWin,2,1, "Sorry, this feature is not yet implemented");
        wmove(errorWin, 0, 1);
        waddch(errorWin, ACS_RTEE);
        wprintw(errorWin, "%s", "Error");
        waddch(errorWin, ACS_LTEE);
        wrefresh(errorWin);
        getch();
        erasewindow(errorWin);
        return;
    }
    





}
