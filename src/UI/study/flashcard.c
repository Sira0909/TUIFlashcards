#include "flashcards.h"
#include <curses.h>
#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
#include <config.h>
#include <poll.h>
#include <animation.h>
#include <UI.h>
//#include <stdlib.h>
//#include <string.h>


struct flashcard_metadata{
    FlashcardSet *flashcard_set;
    int *order;
    int numCards;
    int currentcard;
    int side;
    WINDOW* text;
    WINDOW* FlashcardWindow;
    int maxlength;
};
#define Metadata ((struct flashcard_metadata*)(metadata))
#define currentflashcard Metadata->flashcard_set->cards[Metadata->order[Metadata->currentcard]]
int render_flashcards(void* metadata){
        char* card_text;
        if((/*default_side+*/Metadata->side)%Metadata->flashcard_set->num_columns)
            card_text = currentflashcard.definition[Metadata->side-1];
        else
            card_text = currentflashcard.term;
        werase(Metadata->text);
        touchwin(Metadata->FlashcardWindow);
        wbkgd(Metadata->text, COLOR_PAIR(2));
        wattron(Metadata->text,A_BOLD);
        wprintctr(Metadata->text, (Metadata->maxlength/4)*2-1, Metadata->maxlength+1,  card_text);
        wattroff(Metadata->text,A_BOLD);
        //wprintw(FlashcardWindow, "%d", order[currentcard]);
        printProgress(Metadata->FlashcardWindow, Metadata->currentcard, Metadata->numCards);
        wrefresh(Metadata->FlashcardWindow);
        return 1;

}
int flashcards_flip(void* metadata){
    Metadata->side = (Metadata->side+1)%Metadata->flashcard_set->num_columns;
    return 1;
}
int flashcards_right(void* metadata){
    Metadata->currentcard++;
    if (Metadata->currentcard>=Metadata->numCards){
        Metadata->currentcard = Metadata->numCards-1;
    }
    //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
    Metadata->side = 0;
    return 1;
}
int flashcards_left(void* metadata){
    Metadata->currentcard--;
    if (Metadata->currentcard <0){
        Metadata->currentcard = 0;
    }
    //default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
    Metadata->side = 0;
    return 1;
}
int flashcards_down(void* metadata){
    Metadata->side++;
    if(Metadata->side>=Metadata->flashcard_set->num_columns) Metadata->side=Metadata->flashcard_set->num_columns-1;
    return 1;
}
int flashcards_up(void* metadata){
    Metadata->side--;
    if(Metadata->side<0) Metadata->side=0;
    return 1;
}
int flashcards_star(void*metadata){
        currentflashcard.is_starred = !currentflashcard.is_starred;
        if(currentflashcard.is_starred)
            showmsg("Flashcard has been starred");
        else
            showmsg("Flashcard has been unstarred");
        return 1;
}

void flashcard(FlashcardSet *flashcard_set){

// I dont think this part is necesary tbh for flashcards. but, TODO: add 1d version of this
//  bool starred_only = false;
//  bool shuffle = false;
//  bool (vectorsin)[flashcard_set->num_columns-1];// ___->term
//  bool (vectorsout)[flashcard_set->num_columns-1];// term->____
//  if (!get_settings(flashcard_set, &starred_only, &shuffle,vectorsin, vectorsout)){   
//      return;
//  }
    
    bind_keys(flashcardKeybinds, render_flashcards, 10)
        {config.keylayout.lkey,config.keylayout.str_lkey, "previous",   &flashcards_left},
        {config.keylayout.rkey,config.keylayout.str_rkey, "next",       &flashcards_right},
        {config.keylayout.dkey,config.keylayout.str_dkey, "down",       &flashcards_down},
        {config.keylayout.ukey,config.keylayout.str_ukey, "up",         &flashcards_up},
        {' ',"space", "select", &flashcards_flip},
        {'s', "s", "star current", &flashcards_star},
        {27, " ", " ", &quit},
        {'q', "q / <esc>", " ", &quit},
        {-1, "?", "list keybinds", NULL}
    };



    int order[flashcard_set->num_items];
    int maxlength =25;



    int numCards = getOrder(flashcard_set, order, false, false);

    //no cards match criteria
    if (!numCards)
        return;

    int currentcard = 0;

    WINDOW* FlashcardWindow = create_newwin((maxlength/4)*2+1, maxlength+4, (LINES-maxlength/2)/2, (COLS-maxlength-2)/2);
    WINDOW* text = derwin(FlashcardWindow, (maxlength/4)*2-1, maxlength+2, 1, 1);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    printProgress(FlashcardWindow, currentcard, numCards);
    wrefresh(FlashcardWindow);
    

    int side = 0;
    struct flashcard_metadata metadata = {flashcard_set, order, numCards, currentcard, side, text, FlashcardWindow, maxlength};

    run(&metadata, flashcardKeybinds);

    erasewindow(FlashcardWindow);


    
}
