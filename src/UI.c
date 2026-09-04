#include <ncurses.h>
#include <windows/window.h>
#include <string.h>
#include <UI.h>

void list_keybinds(struct _keybind* binds){
    int numBinds = binds[0].keycode;
    WINDOW* helpwindow = create_newwin(numBinds+4, 32, (LINES-numBinds-2)/2, COLS/2 - 15);
    wbkgd(helpwindow, COLOR_PAIR(2));
    wattron(helpwindow, A_BOLD);

    box(helpwindow, 0, 0);


    mvwprintw(helpwindow, 1, 1, "keybinds:");

    for(int i = 1; i<numBinds; i++){
        mvwprintw(helpwindow, 3+i, 1, "%s", binds[i].name);
        
        mvwprintw(helpwindow, 3+i, 31-strnlen(binds[i].desc,20), "%s", binds[i].desc);
    }

    wrefresh(helpwindow);
    getch();
    erasewindow(helpwindow);

}
int run(void* window, struct _keybind* binds){
    int count = binds[0].keycode;
    int (*render)(void* structure) = binds[0].effect;
    
    while(1){
        render(window);
        int ch = getch();
	if(ch == '?'){//comes before so callers can do post-operations
	    list_keybinds(binds);
	}
        for(int i = 1; i<count; i++){
            if(ch == binds[i].keycode){
                int ret = binds[i].effect(window);
                if(ret !=1){
                    return ret;
                }
            }
        }
    }
}
int quit(void* window){
    return -1;
}
