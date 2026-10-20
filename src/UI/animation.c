#include "config.h"
#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>
#include <animation.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <clock.h>

WINDOW* background;
pthread_t Clock;

void init_Animation(CONFIGSTRUCT config){
    background=stdscr;

    sigset_t mask; sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    int clock_command;
    Clock = setupClock(animation_delay_ms, &clock_command);
    
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
