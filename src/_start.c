#include <locale.h>
#include <string.h>
#include <const.h>
#include <stdio.h>

#ifndef _WIN32
#include <termios.h>
#endif
#include <unistd.h>
#include <ncurses.h>
#include <form.h>
#include <main.h>


#include <config.h>


CONFIGSTRUCT config;

int main(int argc, char *argv[]){

    init_ncurses();
    if(process_flags(argc, argv)==true){
        return 0;
    }
    

    
    int conferrors = get_config_struct(&config);
    if(conferrors>0){
        printf("there are %d errors in your config file. certain values might not be what you want", conferrors);
        getc(stdin);
    }


    
    main_menu(config);

    // clean up
    endwin();
}



bool process_flags(int argc, char *argv[]){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"--version")==0 || strcmp(argv[i],"-v")==0){
            printf("TUIFlashcards version %s\n", ReleaseVersion);
            return true;
        }
        if(strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0){
            printf("Usage: \n\t%s [flags]\n\n", argv[0]);
            printf("Flags:\n");
            printf("\t--version, -v\t\tPrint version information\n");
            printf("\t--help, -h   \t\tPrint this help screen\n");
            return true;
        }
    }
    return false;
}

void init_ncurses(){
    setlocale(LC_ALL, "");
    // init ncurses
    initscr();

    //start color
    start_color();



    if (can_change_color()){
        // colorscheme based on tokyonight
        init_color(COLOR_BLACK, 106, 114, 169);
        init_color(COLOR_RED, 1000, 459,498);
        init_color(COLOR_YELLOW, 1000, 780, 467);
        init_color(COLOR_BLUE, 510, 667, 1000);
        init_color(COLOR_WHITE, 510, 545, 722);

    }

    // colors
    init_pair(1, COLOR_YELLOW , COLOR_BLUE);     // background:         yellow on blue
    init_pair(2, COLOR_BLACK , COLOR_WHITE);    // default window:      black on white
    init_pair(3, COLOR_BLACK , COLOR_RED);      // selection:           black on red
    init_pair(4, COLOR_YELLOW, COLOR_WHITE);      // starred:           yellow on white
    init_pair(5, COLOR_YELLOW, COLOR_RED);      // starred selected:    yellow on red
    init_pair(6, COLOR_BLUE, COLOR_WHITE);      // selection:           white on red
    init_pair(7, COLOR_YELLOW,COLOR_RED);      // error:                red on white
    init_pair(8, COLOR_WHITE , COLOR_BLUE);     // errased window:      white on blue
    init_pair(9, COLOR_BLACK , COLOR_GREEN);     // correct:            black on green
    init_pair(10, COLOR_BLACK , COLOR_RED);      // incorrect:          black on red

    // set ncurses modes
    cbreak();//change later
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    set_escdelay(100);

#ifndef _WIN32
    //allow ctrl+s
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    //term.c_cc[VMIN] = 1;
    term.c_cc[VSTOP] = _POSIX_VDISABLE;
    term.c_cc[VSUSP] = _POSIX_VDISABLE;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
    // set background
    bkgd(COLOR_PAIR(1));

    refresh();
}
