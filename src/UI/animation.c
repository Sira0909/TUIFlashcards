#include <ncurses.h>
#include <stdlib.h>
#include <animation.h>

void init_Animation(void){
    for(int i = 0 ; i < LINES; i++){
        updateAnimation();
    }

    refresh();
}

void updateAnimation(void){
    scrl(-1);
    int numchars = COLS/15;
    char line[COLS+1];
    for(int i = 0; i < COLS; i++){
        line[i]=' ';
    }
    line[COLS]=0;
    for(int i=0; i <numchars; i++){
        line[rand()%COLS] = 'O';
    }
    mvprintw(0,0,"%s", line);
}
