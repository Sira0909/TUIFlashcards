#include <study/modes.h>
#include <windows/window.h>
#include <ncurses.h>
#define complete 0
void multipleChoice(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){

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
