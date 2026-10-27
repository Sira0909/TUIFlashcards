#include "config.h"
#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <animation.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <clock.h>

//WINDOW* background;
pthread_t Clock;

void init_Animation(CONFIGSTRUCT config){
    //background=stdscr;

    sigset_t mask; sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    int clock_command;
    Clock = setupClock(animation_delay_ms, &clock_command);
    
    scrollok(stdscr,false);
    resize_animation();
    for(int i = 0 ; i < LINES; i++){
        updateAnimation(config);
    }

    wnoutrefresh(stdscr);
}
char** background;
int br, bc;
void resize_animation(){
    if(background!=NULL){
        for(int i = 0; i < br; i++){
            free(background[i]);
        }
        free(background);
    }
    br=LINES;
    bc=COLS;
    background=calloc(br,sizeof(char*));
    for(int i = 0 ; i < br; i++){
        background[i]=calloc(bc+1,sizeof(char));
    }
    for(int i = 0 ; i < br; i++){
        updateAnimation(config);
    }
    wnoutrefresh(stdscr);
}

void updateAnimation(CONFIGSTRUCT config){
    if(config.animations){
        for(int i = br-1; i > 0;i--){
            strcpy(background[i],background[i-1]);
        }
        int numchars = bc/15;
        for(int i = 0; i <= bc; i++){
            background[0][i]=' ';
        }
        for(int i=0; i <numchars; i++){
            background[0][rand()%COLS] = 'O';
        }
        //IMPORTANT: this prevents ncurses from realizing we are just scrolling the window.
        // if it knows, it will call scroll, and scroll appears to break the thing and cause heavy flickering.
        for(int i=0; i < bc;i++){
            for(int j=0; j < br;j++){
                if(background[j][i]=='O'){
                    background[j][i]='X';
                }
                else if(background[j][i]=='X'){
                    background[j][i]='O';
                }
            

            }
        }
        background[0][COLS-1]=0;
        for(int i = 1; i < br-1; i++){
            mvprintw(i,0,"%s", background[i]);
        }
        wnoutrefresh(stdscr);
    }
}
