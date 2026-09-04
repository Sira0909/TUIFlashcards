
#include <macros.h>
#include <flashcards.h>
#include <config.h>

#include <windows/window.h>
#include <windows/menu.h>

#include <study.h>
#include <UI.h>







WINDOW* keybindHelp;


int main_menu_select(void * menu);
int main_menu_keybinds(void* menu);

void main_menu(){
    // keybind helper window
    if(config.showKeybindsHelp){
        keybindHelp = create_newwin(3, 18, LINES-4, (COLS-18)/2);
        wattron(keybindHelp, A_BOLD | COLOR_PAIR(5));
        box(keybindHelp, 0,0);
        mvwprintw(keybindHelp, 1, 1, "'?' for keybinds");
        wrefresh(keybindHelp);
    }


    bind_keys(mainkeybinds, render_Menu, 7)
        {config.keylayout.dkey, config.keylayout.str_dkey,"down", &menu_down},
        {config.keylayout.ukey, config.keylayout.str_ukey,"up", &menu_up},
        {10, "<enter>", "select", &main_menu_select},
        {27, "", "", &quit},
        {'q', "q", "quit", &quit},
        {-1, "?", "List Keybinds", NULL}
    };


    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU mainmenu;

    // list of options
    char items[8][128] = {"Study\0", "\0", "New List\0", "Edit List\0", "\0", "Settings", "Quit\0", "\0"};

    // create window for menu. this menu object is defined globally, see above
    WINDOW* menu_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    // init the menu
    init_Menu(&mainmenu, 8, 20, 8, &menu_window, "Let's Study!", NULL, items);

    //this is to allow get_settings to rebind keybinds
    int rerun = 2; 
    while(rerun == 2){
        rerun = run(&mainmenu,mainkeybinds);
        if(rerun==2){
            mainkeybinds[1].keycode =config.keylayout.dkey;
            mainkeybinds[2].keycode =config.keylayout.ukey;
        }
    }
    
    
    // clean up
    delwin(menu_window);

}


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
            addList(config.flashcard_dir);
            break;
        case 3: // "Edit List"
            getLists(editList);
            break;
        case 5:
            get_global_settings();
            return 2;//tells main to rerun run and update keybinds
            break;
        case 6: // "Quit"
            return -1;
            break;
        default:// error, debug info will be printed
            mvwprintw(stdscr, 1,1, "error: accidentally picked %d", ((MENU*)menu)->selected);
            break;
    }
    return 1;
}
