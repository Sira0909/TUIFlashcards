#include "main.h"
#include <ncurses.h>
#include <string.h>


void flashcard(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){
    srand(time(NULL));

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
    WINDOW* FlashcardWindow = create_newwin((maxlength/4)*2+1, maxlength+4, (LINES-maxlength/2)/2, (COLS-maxlength-4)/2);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    wrefresh(FlashcardWindow);


    int currentcard = 0;
    int side = 0;
    int ch = -1;
    bool done = false;
    while (!done){
        char* card_text;
        if(side)
            card_text = flashcard_set->cards[order[currentcard]].definition;
        else
            card_text = flashcard_set->cards[order[currentcard]].name;
        werase(FlashcardWindow);
        wbkgd(FlashcardWindow, COLOR_PAIR(2));
        box(FlashcardWindow, 0, 0);
        wattron(FlashcardWindow,A_BOLD);
        mvwprintw(FlashcardWindow, (maxlength)/4, (maxlength+3-strlen(card_text))/2, "%s", card_text);
        wattroff(FlashcardWindow,A_BOLD);
        //wprintw(FlashcardWindow, "%d", order[currentcard]);
        wrefresh(FlashcardWindow);

        ch = getch();
        switch (ch){
            case 'q':
                erasewindow(FlashcardWindow);
                return;
            case ' ':
                side = !side;
                break;
            case 'l':
                currentcard++;
                if (currentcard>=numCards){
                    currentcard = numCards-1;
                }
                side = 0;
                break;
            case 'h':
                currentcard--;
                if (currentcard <0){
                    currentcard = 0;
                }
                side = 0;
                break;
            case 'j':
                side = 1;
                break;
            case 'k': 
                side = 0;
                break;
        }
    }

    erasewindow(FlashcardWindow);


    
}
