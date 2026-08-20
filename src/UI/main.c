#include <stdlib.h>

#include <macros.h>
#include <flashcards.h>
#include <config.h>

#include <windows/window.h>
#include <windows/menu.h>

#include <study.h>
#include <UI.h>







char *(mainkeybinds[5][2]) = {
    {config.keylayout.str_dkey,"down"},
    {config.keylayout.str_ukey,"up"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};

WINDOW* keybindHelp;


int main_menu_quit(void* menu);
int main_menu_select(void * menu);
int main_menu_keybinds(void* menu);

void main_menu(CONFIGSTRUCT config){
    // keybind helper window
    if(config.showKeybindsHelp){
        keybindHelp = create_newwin(3, 18, LINES-4, (COLS-18)/2);
        wattron(keybindHelp, A_BOLD | COLOR_PAIR(5));
        box(keybindHelp, 0,0);
        mvwprintw(keybindHelp, 1, 1, "'?' for keybinds");
        wrefresh(keybindHelp);
    }




    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU mainmenu;

    // list of options
    char items[8][128] = {"Study\0", "\0", "New List\0", "Edit List\0", "\0", "Settings", "Quit\0", "\0"};

    // create window for menu. this menu object is defined globally, see above
    WINDOW* menu_window = create_newwin(9, 22, (LINES - 7)/2, (COLS - 20)/2);

    // init the menu
    init_Menu(&mainmenu, 8, 20, 8, &menu_window, "Let's Study!", NULL, items);
    
    // add commands to menu, see below and menu.c
    addHook_Menu(&mainmenu, (struct hook){config.keylayout.dkey, &menu_down});
    addHook_Menu(&mainmenu, (struct hook){config.keylayout.ukey, &menu_up});
    addHook_Menu(&mainmenu, (struct hook){'q', &main_menu_quit});
    addHook_Menu(&mainmenu, (struct hook){27,  &main_menu_quit});
    addHook_Menu(&mainmenu, (struct hook){10,  &main_menu_select});
    addHook_Menu(&mainmenu, (struct hook){'?', &main_menu_keybinds});


    // run
    run_Menu(&mainmenu);
    free(mainmenu.hooks);
    
    
    // clean up
    delwin(menu_window);

}

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
            addList(config.flashcard_dir);
            break;
        case 3: // "Edit List"
            getLists(editList);
            break;
        case 5:
            get_global_settings();
            ((MENU*)menu)->hooks[0].trigger = config.keylayout.dkey;
            ((MENU*)menu)->hooks[1].trigger = config.keylayout.ukey;
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
//display keybinds
int main_menu_keybinds(void* menu){list_keybinds(5, mainkeybinds); return 1;}
