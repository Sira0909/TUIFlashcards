#include "play.h"
#include "window.h"
#include <ncurses.h>
void play(char* list){
    WINDOW* mainPlayWindow = create_newwin(20, 75, (LINES-23)/2, (COLS-74)/2);
    wbkgd(mainPlayWindow, COLOR_PAIR(2));
    box(mainPlayWindow, 0, 0);

    WINDOW* topleft = derwin(mainPlayWindow, 9, 23, 1, 2);
    WINDOW* top = derwin(mainPlayWindow, 9, 23, 1, 26);
    WINDOW* topright = derwin(mainPlayWindow, 9, 23, 1, 50);
    WINDOW* bottomleft = derwin(mainPlayWindow, 9, 23, 10, 2);
    WINDOW* bottom = derwin(mainPlayWindow, 9, 23, 10, 26);
    WINDOW* bottomright = derwin(mainPlayWindow, 9, 23, 10, 50);

    wbkgd(topleft, COLOR_PAIR(3));

    box(topleft, 0, 0);
    box(top, 0, 0);
    box(topright, 0, 0);
    box(bottomleft, 0, 0);
    box(bottom, 0, 0);
    box(bottomright, 0, 0);

    wattron(topleft, A_BOLD);
    wattron(top, A_BOLD);
    wattron(topright, A_BOLD);
    wattron(bottomleft, A_BOLD);
    wattron(bottom, A_BOLD);
    wattron(bottomright, A_BOLD);

    mvwprintw(topleft, 4, (23- 10)/2, "flashcards" );
    mvwprintw(top, 4, (23 - 4)/2, "type" );




    wattron(topleft, A_BOLD);
    wrefresh(mainPlayWindow);

    int selectedx = 0;
    int selectedy = 0;

    WINDOW *games[2][3] = { {topleft, top, topright}, {bottomleft, bottom, bottomright} };

    int ch;
    while ((ch = getch())){
        
        switch(ch){
            case 'h': 
                selectedx = (selectedx - 1);
                if (selectedx < 0) selectedx = 2;
                break;
            case 'j':
                selectedy = (selectedy - 1) % 3;
                if (selectedy < 0) selectedy = 1;
                break;
            case 'k':
                selectedy = (selectedy + 1) % 3;
                if (selectedy > 1) selectedy = 0;
                break;
            case 'l':
                selectedx = (selectedx + 1) % 3;
                if (selectedx > 2) selectedx = 0;
                break;
            case 'q':
                erasewindow(mainPlayWindow);
                return;
        }
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 3; j++){
                if(i == selectedy && j == selectedx){
                    wbkgd(games[i][j], COLOR_PAIR(3));
                } else
                    wbkgd(games[i][j], COLOR_PAIR(2));
                wrefresh(games[i][j]);
            }
        }
        
        wrefresh(mainPlayWindow);
    }
    getch();
    erasewindow(mainPlayWindow);
}
