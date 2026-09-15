//
//repeatable UI elements, such as messages, confirmation boxes, and menus, go here.
//




#include <macros.h>
#include <stdlib.h>
#include <string.h>

#define _XOPEN_SOURCE 600
#include <ncurses.h>
#include <form.h>
#include <config.h>

#include <UI.h>
#include <windows/window.h>
#include <windows/menu.h>




void showmsg(char* msg){
    int mlen = strnlen(msg, 128);
    int width = mlen+4;
    WINDOW* msgWindow = newwin(3, width,(LINES-1)/2, (COLS-(width-2))/2);
    wbkgd(msgWindow, COLOR_PAIR(2));
    box(msgWindow,0,0);
    wprintctrx(msgWindow,1, width,msg);
    wrefresh(msgWindow);
    getch();
    erasewindow(msgWindow);
    return;

}
int getConfirmation(char *question, char* successmsg, char* failmsg){
    int qlen = strnlen(question, 128);
    int width = max(qlen, 8)+4;
    WINDOW* confirmWindow = newwin(7, width,(LINES-5)/2, (COLS-(width-2))/2);
    wbkgd(confirmWindow, COLOR_PAIR(2));
    box(confirmWindow,0,0);
    

    wprintctrx(confirmWindow,2, width,question);
    int ctr = (width)/2;
    mvwprintw(confirmWindow,4,ctr-4, "yes");
    mvwprintw(confirmWindow,4,ctr+1, "no");
    mvwchgat(confirmWindow,4,ctr-4,3,A_BOLD, 3,NULL);
    mvwchgat(confirmWindow,4,ctr+1,2,A_NORMAL, 2,NULL);
    int ch=0;
    bool selection = true;
    while (ch!= 10){
        touchwin(confirmWindow);
        wrefresh(confirmWindow);
        ch = getch();
        if(ch == config.keylayout.dkey|| ch ==config.keylayout.lkey){
            selection= false;
            mvwchgat(confirmWindow,4,ctr-4,3,A_NORMAL, 2,NULL);
            mvwchgat(confirmWindow,4,ctr+1,2,A_BOLD, 3,NULL);
        }
        if(ch == config.keylayout.ukey|| ch ==config.keylayout.rkey){
            selection = true;
            mvwchgat(confirmWindow,4,ctr-4,3,A_BOLD, 3,NULL);
            mvwchgat(confirmWindow,4,ctr+1,2,A_NORMAL, 2,NULL);
        }
        if(ch == 'q' ||ch == 27){
            selection = false;
            break;
        }
    }
    if(selection&&successmsg!=NULL){
        showmsg(successmsg);
    }else if (!selection&&failmsg!=NULL){
        showmsg(failmsg);
    }
    erasewindow(confirmWindow);
    return selection;
}

char* getString(char* title, int maxsize, char* startingText){
    WINDOW *my_form_win;
    FIELD *FileNameField[2];
    FORM *Form;
    int ch, rows, cols;



    //set up form box

    //create 1 element form
    FileNameField[0] = new_field(1, 30, 0, 0, (maxsize - 1)/30, 0);
    FileNameField[1] = NULL;

    // set field atributes
    set_field_back(FileNameField[0], COLOR_PAIR(3));
    field_opts_off(FileNameField[0], O_AUTOSKIP);
    field_opts_off(FileNameField[0], O_WRAP);

    //create form
    Form = new_form(FileNameField);
    
    //assign window
    scale_form(Form, &rows, &cols);
    int boxwidth = max(30,strlen(title)); 
    my_form_win = newwin(rows+4, boxwidth+4,(LINES - rows-2)/2,(COLS - boxwidth-2)/2);

    // if we have starting text, use it
    if(startingText!=NULL){
        set_field_buffer(FileNameField[0], 0, startingText);
    }

    set_form_win(Form, my_form_win);
    set_form_sub(Form, derwin(my_form_win, rows, cols, 3, (boxwidth+4-cols)/2));
    
    //set form background
    wbkgd(my_form_win, COLOR_PAIR(2));


    box(my_form_win, 0, 0);
    //wmove(my_form_win, 0, 1); waddch(my_form_win, ACS_RTEE);wprintw(my_form_win, "%s", "double escape to cancel"); waddch(my_form_win, ACS_LTEE);
    mvwprintw(my_form_win, 1, (boxwidth+4-strlen(title))/2, "%s", title);

    post_form(Form);
    form_driver(Form, REQ_END_FIELD);
    wrefresh(my_form_win);

    curs_set(1);

    //for accents
    bool wasJustBacktick = false;
    bool wasJustTilde = false;

    while((ch = getch())){
        switch (ch){
            case 10: //enter
                //do return stuff
                form_driver(Form, REQ_NEXT_FIELD);
                form_driver(Form, REQ_PREV_FIELD);
                char* string = calloc(maxsize+1, sizeof(char));
                string = strncpy(string, field_buffer(FileNameField[0],0), maxsize);
                int end = maxsize-1;
                //trims the whitespace from end of form
                while(string[--end] == '\0' || string[end] == ' '){
                }
                string[end+1] = '\0';
                
                //turn cursor back off
                curs_set(0);

                //cleanup
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                erasewindow(my_form_win);
                refresh();
                return string;
                
            case 27: //cancel (esc key)

                curs_set(0);
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                wborder(my_form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
                wbkgd(my_form_win, COLOR_PAIR(1));
                wrefresh(my_form_win);
                delwin(my_form_win);
                refresh();
                return NULL;

            //move cursor
            case KEY_LEFT:
                    form_driver(Form, REQ_PREV_CHAR);
                    wasJustBacktick = false;
                    wasJustTilde = false;
                    break;

            case KEY_RIGHT:
                    form_driver(Form, REQ_NEXT_CHAR);
                    wasJustBacktick = false;
                    wasJustTilde = false;
                    break;

            // delete before cursor
            case KEY_BACKSPACE:
            case 127:
                form_driver(Form, REQ_DEL_PREV);
                wasJustBacktick = false;
                wasJustTilde = false;
                break;

            // delete under cursor
            case KEY_DC:
                form_driver(Form, REQ_DEL_CHAR);
                wasJustBacktick = false;
                wasJustTilde = false;
                break;

            handleAccents(Form)
        }
        wrefresh(my_form_win);
    }
    curs_set(0);

    unpost_form(Form);
    free_form(Form);
    free_field(FileNameField[0]);
    wborder(my_form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
    wbkgd(my_form_win, COLOR_PAIR(1));
    wrefresh(my_form_win);
    delwin(my_form_win);
    refresh();

    return NULL;
}
int _select(void* menu){return 2; (void)menu;}

int makeSelection(int numOptions, char (*options)[128], char* title){
    bind_keys(menukeybinds, render_Menu, 7)
        {config.keylayout.dkey, config.keylayout.str_dkey,"down", &menu_down},
        {config.keylayout.ukey, config.keylayout.str_ukey,"up", &menu_up},
        {10, "<enter>", "select", &_select},
        {27, "", "", &quit},
        {'q', "q", "quit/cancel", &quit},
        {-1, "?", "List Keybinds", NULL}
    };


    // create MENU object for main menu (see MENU.c, MENU.h)
    MENU menu;


    // create window for menu. this menu object is defined globally, see above
    WINDOW* menu_window = create_newwin(numOptions+3, 34, (LINES - numOptions)/2, (COLS - 32)/2);

    // init the menu
    init_Menu(&menu, numOptions, 34, numOptions+3, &menu_window, title, NULL, options);

    int res = run(&menu,menukeybinds);
    delwin(menu_window);
    if(res == -1){
        return -1;
    }
    else{
        return menu.selected;
    }
}
