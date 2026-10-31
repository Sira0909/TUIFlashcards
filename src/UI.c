#include <curses.h>
#include <ncurses.h>
#include <animation.h>
#include <signal.h>
#include <unistd.h>
#include <sys/signalfd.h>
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
    int count = binds[0].keycode; //number of keybinds to handle
    int (*render)(void* structure) = binds[0].effect;

    // set up signals from animation.
    sigset_t mask; sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    struct pollfd pfds[2];
    // we want poll to listen for stdin and the signal from animation
    pfds[0].fd = 0; //stdin
    pfds[0].events = POLLIN;
    pfds[1].fd = signalfd(-1, &mask,0);//animation
    pfds[1].events = POLLIN;
    //to detect terminal window resizing
    int l=LINES;
    int c=COLS;
    
    while(1){
        do{
            if(is_term_resized(l, c)){
                l=LINES;
                c=COLS;
                resize_animation();
                ungetch(KEY_RESIZE);//so that getch has something to read after break
                break;
            }
            if(pfds[1].revents&POLLIN){ //signaled by animation
                struct signalfd_siginfo ignore;
                read(pfds[1].fd, &ignore, sizeof(ignore));
                updateAnimation(config);
            }
            render(window);
            doupdate();
            poll(pfds,2, 100);
        }
        while(!(pfds[0].revents&POLLIN));
        int ch = getch();
	if(ch == '?'){//comes before so callers can do post-operations
	    list_keybinds(binds);
	}
        for(int i = 1; i<count; i++){
            if(ch == binds[i].keycode){
                int ret = binds[i].effect(window);
                if(ret !=1){ // 1 on success and continue. typically, I use 0 for regular quit and 2 for special purposes such as main_menu needing to refresh its keybinds after settings change.
                    return ret;
                }
            }
        }
    }
}
int quit(__attribute__((unused)) void* window){
    return -1;
}
