#include "config.h"
#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>
#include <animation.h>

WINDOW* background;

void init_Animation(CONFIGSTRUCT config){
    background=newwin(LINES, COLS, 0, 0);
    
    wbkgd(background, COLOR_PAIR(1));
    scrollok(background,true);
    for(int i = 0 ; i < LINES; i++){
        updateAnimation(config);
    }

    wrefresh(background);
}

void updateAnimation(CONFIGSTRUCT config){
    if(config.animations){
        wscrl(background,-2);
        int numchars = COLS/15;
        char line[COLS+1];
        for(int i = 0; i < COLS; i++){
            line[i]=' ';
        }
        line[COLS]=0;
        for(int i=0; i <numchars; i++){
            line[rand()%COLS] = 'O';
        }
        mvwprintw(background,0,0,"%s", line);
        wnoutrefresh(background);
    }
}
