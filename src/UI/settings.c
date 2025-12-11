//unfinished
#include <ncurses.h>
#include <windows/table.h>
#include <windows/window.h>
#include <stdlib.h>
#include <string.h>
#include <config.h> 
#include <UI.h>




char global_settings_keybinds[10][2][20] = {
    {"j","down"},
    {"k","up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {" ", " "},
    {"?", "list keybinds"}
};
CONFIGSTRUCT newConfig;

int settings_keybinds(void* menu){
    list_keybinds(6, global_settings_keybinds);                         return 1;
}

int settings_quit(void* Table){
    return -1;
}
int settings_save(void* Table){
    config = newConfig;
    if(writeconfig()==-1){
        WINDOW* errorWin = create_newwin(3, 25, (LINES-1)/2, (COLS-23)/2);
        wbkgd(errorWin, COLOR_PAIR(7));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);

        mvwprintw(errorWin,1,1, "Failed to save settings");

        wrefresh(errorWin);
        getch();
        return 1;


    }
    return -1;
     
}
int settings_select(void* table){
    TABLE* settingstable = table;
    switch(settingstable->selected_row){
        case 0:
            newConfig.autoaccent = !newConfig.autoaccent;
            settingstable->highlighted[0] = newConfig.autoaccent ? '*' : 0;
            strcpy(settingstable->table_data[1][0], (config.autoaccent) ? "On": "Off");
            break;
        case 1:
            newConfig.showKeybindsHelp = !newConfig.showKeybindsHelp;
            settingstable->highlighted[1] = newConfig.showKeybindsHelp ? '*' : 0;
            strcpy(settingstable->table_data[1][1], (config.showKeybindsHelp) ? "On": "Off");
            break;
        case 3:
            // clean up
            settings_save(table);
            return -1;
        case 4:
            return -1;
    }
    return 1;
}


//get settings 
bool get_global_settings(){
    newConfig = config;
    TABLE settings_table;

    int height = 6;
    int width = 60;
    // create window for menu. 
    WINDOW* setting_window = create_newwin(height+2, 35, (LINES - height)/2, (COLS - 33)/2);
    WINDOW* tablewindow = derwin(setting_window, height, 33, 1, 1);

    char items[6][128] = { "Automatic accents", "Show how to access keybinds", "\0", "Save\0", "Cancel"};
    char value[6][128]; 
    strcpy(value[0], (config.autoaccent) ? "On": "Off");
    strcpy(value[1], (config.showKeybindsHelp) ? "On": "Off");
    char (*(table[2]))[128] = {items, value};

    char selected[6] = {0,0,0,0,0,0};
    selected[0] = (config.autoaccent) ? '*': ' ';
    selected[1] = (config.showKeybindsHelp) ? '*': ' '; 


    char headers[2][128] = {"Setting", ""};

    wbkgd(setting_window, COLOR_PAIR(2));
    box(setting_window, 0, 0);

    wrefresh(setting_window);

    init_Table(&settings_table, 5, 2, width, height, &tablewindow, "Settings", headers, table, selected);
    wrefresh(settings_table.window);

     

    addHook_Table(&settings_table, (struct hook){'j', &table_down});
    addHook_Table(&settings_table, (struct hook){'k', &table_up});
    addHook_Table(&settings_table, (struct hook){27, &settings_quit});
    addHook_Table(&settings_table, (struct hook){'q', &settings_quit});
    addHook_Table(&settings_table, (struct hook){'?', &settings_keybinds});
    addHook_Table(&settings_table, (struct hook){10, &settings_select});
    run_Table(&settings_table);
    free(settings_table.hooks);

    //cleanup
    erasewindow(tablewindow);
    erasewindow(setting_window);
    tablewindow = NULL;
    settings_table.window = NULL;
    refresh();
    return 1;
}
