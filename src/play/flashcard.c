#include "main.h"
#include <ncurses.h>

void flashcard(FlashcardSet *flashcard_set){
    bool star_only  =   false;
    bool shuffle_on =   false;

    WINDOW* FlashcardWindow = create_newwin(LINES-5, COLS-5, 2, 2);
    wbkgd(FlashcardWindow, COLOR_PAIR(2));
    box(FlashcardWindow, 0, 0);

    wrefresh(FlashcardWindow);


    int currentcard = 0;
    int ch = -1;
    getch();
    erasewindow(FlashcardWindow);
}
