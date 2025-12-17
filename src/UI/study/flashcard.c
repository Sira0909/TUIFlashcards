#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
#include <stdlib.h>
//#include <string.h>


void flashcard(FlashcardSet *flashcard_set){

    bool starred_only = false;
    bool shuffle = false;
    int vectors = 0;
    if (!get_settings(&starred_only, &shuffle,&vectors)){
        return;
    }



    int order[flashcard_set->num_items];
    int maxlength =25;



    int numCards = getOrder(flashcard_set, order, shuffle, starred_only);

    //no cards match criteria
    if (!numCards)
        return;

    WINDOW* FlashcardWindow = create_newwin((maxlength/4)*2+1, maxlength+4, (LINES-maxlength/2)/2, (COLS-maxlength-4)/2);
    WINDOW* text = derwin(FlashcardWindow, (maxlength/4)*2-1, maxlength+2, 1, 1);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    wrefresh(FlashcardWindow);


    int currentcard = 0;
    int default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
    int side = 0;
    int ch = -1;
    bool done = false;
    while (!done){
        char* card_text;
        if((default_side+side)%2)
            card_text = flashcard_set->cards[order[currentcard]].definition;
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
                side = !side;
                break;
            case 'l':
                currentcard++;
                if (currentcard>=numCards){
                    currentcard = numCards-1;
                }
                default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
                side = 0;
                break;
            case 'h':
                currentcard--;
                if (currentcard <0){
                    currentcard = 0;
                }
                default_side = (vectors==0) ? 0 : (vectors==1) ? 1 : rand()%2;
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
