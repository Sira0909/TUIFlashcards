#include <ncurses.h>
#include <animation.h>
#include <windows/window.h>
#include <string.h>
#include <UI.h>
#include <poll.h>

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

    struct pollfd pfds[1];
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    
    while(1){
        do{
            render(window);
            poll(pfds,1, 500);
            updateAnimation();
            refresh();
        }
        while(!(pfds[0].revents&POLLIN));
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
int quit(__attribute__((unused)) void* window){
    return -1;
}
