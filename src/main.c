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

WINDOW* keybindHelp;


int main_menu_j(void* menu);
int main_menu_k(void* menu);
int main_menu_quit(void* menu);
int main_menu_select(void * menu);
int main_menu_keybinds(void* menu);

int main(){

    int conferrors = get_config_struct(&config);
    if(conferrors>0){
        printf("there are %d errors in your config file. certain values might not be what you want", conferrors);
        getc(stdin);
    }


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

    // keybind helper window
    keybindHelp = create_newwin(3, 18, LINES-4, (COLS-18)/2);
    wattron(keybindHelp, A_BOLD | COLOR_PAIR(5));
    box(keybindHelp, 0,0);
    mvwprintw(keybindHelp, 1, 1, "'?' for keybinds");
    refresh();
    wrefresh(keybindHelp);

    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU mainmenu;

    // list of options
    char items[7][128] = {"Study\0", "\0", "New List\0", "Edit List\0", "\0", "Quit\0", "\0"};

    // create window for menu. this menu object is defined globally, see above
    WINDOW* menu_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    // init the menu
    init_Menu(&mainmenu, 7, 20,7, &menu_window, "Let's Study!", NULL, items);
    

    // add commands to menu, see below and menu.c
    addHook_Menu(&mainmenu, (struct hook){'j', &main_menu_j});
    addHook_Menu(&mainmenu, (struct hook){'k', &main_menu_k});
    addHook_Menu(&mainmenu, (struct hook){'q', &main_menu_quit});
    addHook_Menu(&mainmenu, (struct hook){27,  &main_menu_quit});
    addHook_Menu(&mainmenu, (struct hook){10,  &main_menu_select});
    addHook_Menu(&mainmenu, (struct hook){'?', &main_menu_keybinds});


    // run
    run_Menu(&mainmenu);
    free(mainmenu.hooks);
    
    
    // clean up
    delwin(menu_window);


    // clean up
    endwin();
}



//see menu.c for how these work

// j moves down
int main_menu_j(void* menu){changeselect_Menu((MENU*) menu, 1); return 1;}
// k moves up
int main_menu_k(void* menu){changeselect_Menu((MENU*) menu, -1); return 1;}
// when quit
int main_menu_quit(void* menu){return -1;}

// select option
int main_menu_select(void * menu){
    if (keybindHelp != NULL) // erase keybinds now
        erasewindow(keybindHelp);
    keybindHelp=NULL;
    switch(((MENU*)menu)->selected){
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
            return -1;
            break;
        default:// error, debug info will be printed
            mvwprintw(stdscr, 1,1, "error: accidentally picked %d", ((MENU*)menu)->selected);
            break;
    }
    return 1;
}
//display keybinds
int main_menu_keybinds(void* menu){list_keybinds(7, mainkeybinds); return 1;}
