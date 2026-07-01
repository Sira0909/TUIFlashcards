#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
//#include <stdlib.h>
//#include <string.h>


void flashcard(FlashcardSet *flashcard_set){

// I dont think this part is necesary tbh for flashcards. but, TODO: add 1d version of this
//  bool starred_only = false;
//  bool shuffle = false;
//  bool (vectorsin)[flashcard_set->num_columns-1];// ___->term
//  bool (vectorsout)[flashcard_set->num_columns-1];// term->____
//  if (!get_settings(flashcard_set, &starred_only, &shuffle,vectorsin, vectorsout)){   
//      return;
//  }
    



    int order[flashcard_set->num_items];
    int maxlength =25;



    int numCards = getOrder(flashcard_set, order, false, false);

    //no cards match criteria
    if (!numCards)
        return;

    WINDOW* FlashcardWindow = create_newwin((maxlength/4)*2+1, maxlength+4, (LINES-maxlength/2)/2, (COLS-maxlength-4)/2);
    WINDOW* text = derwin(FlashcardWindow, (maxlength/4)*2-1, maxlength+2, 1, 1);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    wrefresh(FlashcardWindow);


    int currentcard = 0;
    //int default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
    int side = 0;
    int ch = -1;
    bool done = false;
    while (!done){
        char* card_text;
        if((/*default_side+*/side)%flashcard_set->num_columns)
            card_text = flashcard_set->cards[order[currentcard]].definition[side-1];
        else
            card_text = flashcard_set->cards[order[currentcard]].term;
        werase(text);
        touchwin(FlashcardWindow);
        wbkgd(text, COLOR_PAIR(2));
        wattron(text,A_BOLD);
        wprintctr(text, (maxlength/4)*2-1, maxlength+1,  card_text);
        wattroff(text,A_BOLD);
        //wprintw(FlashcardWindow, "%d", order[currentcard]);
        wrefresh(FlashcardWindow);


        ch = getch();
        switch (ch){
            case 27:
            case 'q':
                erasewindow(FlashcardWindow);
                return;
            case ' ':
                side = (side+1)%flashcard_set->num_columns;
                break;
            case 'l':
                currentcard++;
                if (currentcard>=numCards){
                    currentcard = numCards-1;
                }
                //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
                side = 0;
                break;
            case 'h':
                currentcard--;
                if (currentcard <0){
                    currentcard = 0;
                }
                //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
                side = 0;
                break;
            case 'j':
                side++;
                if(side>=flashcard_set->num_columns) side=flashcard_set->num_columns-1;
                break;
            case 'k': 
                side--;
                if(side<0) side=0;
                break;
        }
    }

    erasewindow(FlashcardWindow);


    
}
