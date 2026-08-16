#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
#include <config.h>
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

    int currentcard = 0;

    WINDOW* FlashcardWindow = create_newwin((maxlength/4)*2+1, maxlength+4, (LINES-maxlength/2)/2, (COLS-maxlength-4)/2);
    WINDOW* text = derwin(FlashcardWindow, (maxlength/4)*2-1, maxlength+2, 1, 1);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    printProgress(FlashcardWindow, currentcard, numCards);
    wrefresh(FlashcardWindow);


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
        printProgress(FlashcardWindow, currentcard, numCards);
        wrefresh(FlashcardWindow);


        ch = getch();
        if(ch == 27 || ch == 'q'){
            erasewindow(FlashcardWindow);
            return;
        }
        else if(ch == ' '){
            side = (side+1)%flashcard_set->num_columns;
        }
        else if(ch == config.keylayout.rkey){
            currentcard++;
            if (currentcard>=numCards){
                currentcard = numCards-1;
            }
            //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
            side = 0;
        }
        else if(ch == config.keylayout.lkey){
            currentcard--;
            if (currentcard <0){
                currentcard = 0;
            }
            //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
            side = 0;
        }
        else if(ch == config.keylayout.dkey){
            side++;
            if(side>=flashcard_set->num_columns) side=flashcard_set->num_columns-1;
        }
        else if(ch == config.keylayout.ukey){
            side--;
            if(side<0) side=0;
        }
    }

    erasewindow(FlashcardWindow);


    
}
