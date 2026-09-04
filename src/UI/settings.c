//unfinished
#include <curses.h>
#include <ncurses.h>
#include <windows/table.h>
#include <windows/window.h>
#include <string.h>
#include <config.h> 
#include <UI.h>




struct settings_metadata {
    CONFIGSTRUCT newConfig;
    WINDOW* mainWindow;
};
#define Metadata ((struct settings_metadata*)(((TABLE*)Table)->metadata))

char *global_settings_keybinds[10][2] = {
    {config.keylayout.str_dkey,"down"},
    {config.keylayout.str_ukey,"up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {" ", " "},
    {"?", "list keybinds"}
};
//get settings 
int settings_save(void* Table);
int settings_select(void* Table);
int settings_fixAfterKeybinds(void* Table);
TABLE setup_global_settings_table(int width, int height, WINDOW* tablewindow);
bool get_global_settings(){
    bind_keys(global_settings_keybinds, render_Table, 7)
        {config.keylayout.dkey, config.keylayout.str_dkey,"down", &table_down},
        {config.keylayout.ukey, config.keylayout.str_ukey,"up", &table_up},
        {10, "<enter>", "toggle", &settings_select},
        {27, "", "", &quit},
        {'q', "q / <esc>", "quit without saving", &quit},
        {'?', "?", "List Keybinds", &settings_fixAfterKeybinds}

    };

    int height = 6;
    int width = 58;
    // create window for menu. 
    WINDOW* setting_window = create_newwin(height+2, 35, (LINES - height)/2, (COLS - 33)/2);
    WINDOW* tablewindow = derwin(setting_window, height, 33, 1, 1);
    struct settings_metadata metadata = {config, setting_window};

    TABLE settings_table;
    char items[6][128] = { "Automatic accents", "Show how to access keybinds", "Keyboard Layout", "", "Save", "Cancel"};
    char value[6][128] = {"","","","",""}; 
    strcpy(value[0], (config.autoaccent) ? "On": "Off");
    strcpy(value[1], (config.showKeybindsHelp) ? "On": "Off");
    char (*table[2])[128] = {items, value};
    switch(metadata.newConfig.keylayout.layout){
        case KEYBOARD_UNSET:
        case KEYBOARD_ARROW:
            strcpy(value[2], "AROW");
            break;
        case KEYBOARD_IJKL:
            strcpy(value[2], "IJKL");
            break;
        case KEYBOARD_NVIM:
            strcpy(value[2], "NVIM");
            break;
    }

    char selected[6] = {0,0,0,0,0,0};
    selected[0] = (config.autoaccent) ? '*': ' ';
    selected[1] = (config.showKeybindsHelp) ? '*': ' '; 


    char headers[2][128] = {"Setting", ""};
    init_Table(&settings_table, 5, 2, width, height, &tablewindow, "Settings", headers, table, selected);
    settings_table.metadata=&metadata;//for fixing window after keybinds

    wbkgd(setting_window, COLOR_PAIR(2));
    box(setting_window, 0, 0);

    wrefresh(setting_window);

    wrefresh(settings_table.window);

     

    run(&settings_table,global_settings_keybinds);

    //cleanup
    erasewindow(tablewindow);
    erasewindow(setting_window);
    tablewindow = NULL;
    settings_table.window = NULL;
    refresh();
    return 1;
}
int settings_fixAfterKeybinds(void* Table){
    touchwin(((struct settings_metadata*)(((TABLE*)Table)->metadata))->mainWindow);
    wrefresh(((struct settings_metadata*)(((TABLE*)Table)->metadata))->mainWindow);
    
    return 1;
}

int settings_save(void* Table){
    config = Metadata->newConfig;
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
int settings_select(void* Table){
    TABLE* settingstable = Table;
    switch(settingstable->selected_row){
        case 0:
            Metadata->newConfig.autoaccent = !Metadata->newConfig.autoaccent;
            settingstable->highlighted[0] = Metadata->newConfig.autoaccent ? '*' : 0;
            strcpy(settingstable->table_data[1][0], (Metadata->newConfig.autoaccent) ? "On": "Off");
            break;
        case 1:
            Metadata->newConfig.showKeybindsHelp = !Metadata->newConfig.showKeybindsHelp;
            settingstable->highlighted[1] = Metadata->newConfig.showKeybindsHelp ? '*' : 0;
            strcpy(settingstable->table_data[1][1], (Metadata->newConfig.showKeybindsHelp) ? "On": "Off");
            break;
        case 2:
            switch(Metadata->newConfig.keylayout.layout){
                case KEYBOARD_UNSET:
                case KEYBOARD_ARROW:
                    setkeylayout(&Metadata->newConfig, KEYBOARD_IJKL);
                    strcpy(settingstable->table_data[1][2], "IJKL");
                    break;
                case KEYBOARD_IJKL:
                    setkeylayout(&Metadata->newConfig, KEYBOARD_NVIM);
                    strcpy(settingstable->table_data[1][2], "NVIM");
                    break;
                case KEYBOARD_NVIM:
                    setkeylayout(&Metadata->newConfig, KEYBOARD_ARROW);
                    strcpy(settingstable->table_data[1][2], "AROW");
                    break;
            }

            break;
        case 4:
            // clean up
            settings_save(Table);
            return -1;
        case 5:
            return -1;
    }
    return 1;
}


