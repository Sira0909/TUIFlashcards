#include <stdlib.h>

#include <ncurses.h>
#include <form.h>

#include <macros.h>
#include <flashcards.h>
#include <config.h>

#include <windows/window.h>
#include <windows/menu.h>
#include <windows/table.h>

#include <study.h>
#include <UI.h>






CONFIGSTRUCT config;



char mainkeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};

int main(){

    get_config_struct(&config);


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

    // set background
    bkgd(COLOR_PAIR(1));
    attron(A_BOLD);
    attron(COLOR_PAIR(5));
    mvprintw(LINES-4, (COLS-16)/2, "                ");
    mvprintw(LINES-3, (COLS-16)/2, "'?' for keybinds");
    mvprintw(LINES-2, (COLS-16)/2, "                ");
    attroff(COLOR_PAIR(5));
    attroff(A_BOLD);
    refresh();

    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU mainmenu;

    // list of options
    char items[7][128] = {"Study\0", "\0", "New List\0", "Edit List\0", "\0", "Quit\0", "\0"};

    // create window for menu. this menu object is defined globally, see above
    WINDOW* menu_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    // init the menu
    init_menu(&mainmenu, 7, 20,7, &menu_window, "Let's Study!", items);
    wrefresh(mainmenu.window);
    
    // character from getch()
    int ch;

    // so that we can leave while loop
    bool done = false;
    while(!done){
        // refresh menu (see MENU.c)
        render_menu(&mainmenu, NULL) ;
        ch = getch();
        switch(ch){
            case 'j': // down
                changeselect(&mainmenu, 1);
                break;
            case 'k': // up
                changeselect(&mainmenu, -1);
                break;
            case 27:
            case 'q': // quit
                done = true;
                break;
            case 10: // enter
                wmove(stdscr, LINES-3, 0);
                wclrtobot(stdscr);
                wrefresh(stdscr);
                switch(mainmenu.selected){
                    case 0: // "Study"
                        getLists(pickMode);
                        break;
                    case 2: // "New List"
                        addList();
                        break;
                    case 3: // "Edit List"
                        getLists(editList);
                        break;
                    case 5: // "Quit"
                        done = true;
                        break;
                    default:// error, debug info will be printed
                        wprintw(menu_window, "%d", mainmenu.selected);
                        break;
                }
                break;
            case '?':
                list_keybinds(7, mainkeybinds);
                break;
        }
                
    }
    
    // clean up
    mainmenu.window = NULL;
    free(mainmenu.window);
    delwin(menu_window);


    // clean up
    endwin();
}
