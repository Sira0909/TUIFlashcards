#include <ncurses.h>
#include <study.h>
#include <stdlib.h>
#include <windows/window.h>


#include <form.h>
#define complete 1
void type(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){

    int order[flashcard_set->num_items];
    int maxlength =25;



    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, shuffle, starred_only);
    if (!numCards)
        return;

    for(int i = 0; i<numCards;i++){
        if (strlen(flashcard_set->cards[order[i]].name)>maxlength){
            maxlength = strlen(flashcard_set->cards[i].name);
        }
    }

    if(!complete){
        WINDOW* errorWin = create_newwin(5, 46, (LINES-8)/2, (COLS-44)/2);
        wbkgd(errorWin, COLOR_PAIR(2));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);
        mvwprintw(errorWin,2,1, "Sorry, this feature is not yet implemented");
        wmove(errorWin, 0, 1);
        waddch(errorWin, ACS_RTEE);
        wprintw(errorWin, "%s", "Error");
        waddch(errorWin, ACS_LTEE);
        wrefresh(errorWin);
        getch();
        erasewindow(errorWin);
        return;
    }
    
    int currentcard = 0;

    FIELD *FileNameField[2];
    FORM *Form;
    int ch, rows, cols;

    //create 1 element form
    FileNameField[0] = new_field(1, 30, 0, 0, 0, 0);
    FileNameField[1] = NULL;

    // set field atributes
    set_field_back(FileNameField[0], COLOR_PAIR(3));
    field_opts_off(FileNameField[0], O_AUTOSKIP);

    //create form
    Form = new_form(FileNameField);
    
    //assign window
    scale_form(Form, &rows, &cols);
    WINDOW* my_form_win = newwin(13, cols+4,(LINES - 11)/2,(COLS - cols-2)/2);

    set_form_win(Form, my_form_win);
    set_form_sub(Form, derwin(my_form_win, rows, cols, 11, 2));
    
    WINDOW* text = derwin(my_form_win, 10, cols+2, 1, 1);
    //set form background
    wbkgd(my_form_win, COLOR_PAIR(2));


    box(my_form_win, 0, 0);

    post_form(Form);
    werase(text);
    wbkgd(text, COLOR_PAIR(2));
    wattron(text, A_BOLD);
    wprintw(text, "%s", flashcard_set->cards[order[currentcard]].definition);
    wrefresh(my_form_win);

    curs_set(1);


    while((ch = getch())){
        touchwin(my_form_win);
        switch (ch){
            case 10: //enter
                //do return stuff
                form_driver(Form, REQ_NEXT_FIELD);
                form_driver(Form, REQ_PREV_FIELD);
                char* string = calloc(maxlength+2, sizeof(char));
                string = strncpy(string, field_buffer(FileNameField[0],0), maxlength);
                int end = maxlength+1;
                while(string[--end] == '\0' || string[end] == ' '){
                }
                string[end+1] = '\0';
                form_driver(Form, REQ_CLR_FIELD);

                currentcard++;
                if(currentcard>=numCards){
                    currentcard--;
                }
                werase(text);
                wbkgd(text, COLOR_PAIR(2));
                wprintw(text, "%s", flashcard_set->cards[order[currentcard]].definition);
                wrefresh(my_form_win);
                break;
            case 27:

                curs_set(0);
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                wborder(my_form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
                wbkgd(my_form_win, COLOR_PAIR(1));
                wrefresh(my_form_win);
                delwin(my_form_win);
                refresh();
                //return NULL;
                getch();
                // update flashcard and form
                return;


            //move cursor
            case KEY_LEFT:
                    form_driver(Form, REQ_PREV_CHAR);
                    break;

            case KEY_RIGHT:
                    form_driver(Form, REQ_NEXT_CHAR);
                    break;

            // delete before cursor
            case KEY_BACKSPACE:
            case 127:
                form_driver(Form, REQ_DEL_PREV);
                break;

            // delete under cursor
            case KEY_DC:
                form_driver(Form, REQ_DEL_CHAR);
                break;


            default:
                form_driver(Form, ch);
                break;
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

    return ;
}




