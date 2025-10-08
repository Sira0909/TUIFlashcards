#include <ncurses.h>
#include <study.h>
#include <stdlib.h>
#include <windows/window.h>


#include <form.h>

#define currentFlashcard flashcard_set->cards[order[currentcard]]

void printProgress(WINDOW* win, int currentcard, int maxcards){
    wmove(win, 0, 1); 
    mvwaddch(win, 0, 1, ACS_RTEE);
    wprintw(win, "%d/%d", currentcard, maxcards);
    waddch(win, ACS_LTEE);
}
void type(FlashcardSet *flashcard_set, bool starred_only, bool shuffle){

    int order[flashcard_set->num_items];
    int maxlength =25;
    int mistakeorder[flashcard_set->num_items];
    int mistakeindex = 0;



    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, shuffle, starred_only);
    if (!numCards)
        return;

    for(int i = 0; i<numCards;i++){
        if (strlen(flashcard_set->cards[order[i]].name)>maxlength){
            maxlength = strlen(flashcard_set->cards[i].name);
        }
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
    field_opts_off(FileNameField[0], O_WRAP);

    //create form
    Form = new_form(FileNameField);
    
    //assign window
    scale_form(Form, &rows, &cols);
    WINDOW* form_win = newwin(13, cols+4,(LINES - 11)/2,(COLS - cols-2)/2);

    set_form_win(Form, form_win);
    set_form_sub(Form, derwin(form_win, rows, cols, 11, 2));
    
    WINDOW* text = derwin(form_win, 10, cols+2, 1, 1);
    //set form background
    wbkgd(form_win, COLOR_PAIR(2));


    box(form_win, 0, 0);
    printProgress(form_win, currentcard, numCards);

    post_form(Form);
    werase(text);
    wbkgd(text, COLOR_PAIR(2));
    wattron(text, A_BOLD);
    wprintw(text, "%s", currentFlashcard.definition);
    wrefresh(form_win);

    curs_set(1);



    while((ch = getch())){
        touchwin(form_win);
        int x,y;
        getyx(stdscr, y,x);
        move(LINES-3, 0);
        clrtoeol();
        move(y,x);
        switch (ch){
            case 19: // ctrl+s
                if(currentcard>0){
                    attron(COLOR_PAIR(1));
                    flashcard_set->cards[order[currentcard-1]].is_starred = !flashcard_set->cards[order[currentcard-1]].is_starred;
                    wprintctrx(stdscr, LINES-3, COLS, (flashcard_set->cards[order[currentcard-1]].is_starred) ? "Previous flashcard has been starred" : "Previous flashcard has been unstarred");
                    attron(COLOR_PAIR(1));
                    break;
                }
                // stars current when this is first flashcard
                attron(COLOR_PAIR(1));
                currentFlashcard.is_starred = !currentFlashcard.is_starred;
                wprintctrx(stdscr, LINES-3, COLS, (currentFlashcard.is_starred) ? "Flashcard has been starred" : "Flashcard has been unstarred");
                attron(COLOR_PAIR(1));
                break;
            case 10: //enter
                //do return stuff
                form_driver(Form, REQ_NEXT_FIELD);
                form_driver(Form, REQ_PREV_FIELD);

                char* answer = field_buffer(FileNameField[0],0);
                int end = maxlength+1;
                while(answer[--end] == '\0' || answer[end] == ' '){}
                answer[end+1] = '\0';
                form_driver(Form, REQ_CLR_FIELD);

                char *correctanswer=currentFlashcard.name;


                WINDOW* result_win = newwin(13, cols+4,(LINES - 11)/2,(COLS - cols-2)/2);
                if(strcmp(answer, correctanswer)!=0){ // incorrect
                    curs_set(0);
                    wbkgd(result_win, COLOR_PAIR(10));
                    wattron(result_win,A_BOLD);
                    wprintctrx(result_win, 5, cols+4, "Your answer did not match.");
                    wprintctrx(result_win, 6, cols+4, "Press enter to continue, or");
                    wprintctrx(result_win, 7, cols+4, "esc to try again immediately");              
                    wattroff(result_win,A_BOLD);
                    box(result_win,0,0);
                    wrefresh(result_win);
                    int c2 = getch();
                    
                    if(c2== 's'){ //star mistake
                        attron(COLOR_PAIR(1));
                        currentFlashcard.is_starred = !flashcard_set->cards[order[currentcard]].is_starred;
                        wprintctrx(stdscr, LINES-3, COLS, (currentFlashcard.is_starred) ? "Flashcard has been starred" : "Flashcard has been unstarred");
                        attron(COLOR_PAIR(1));
                        c2 = getch();
                        getyx(stdscr, y,x);
                        move(LINES-3, 0);
                        clrtoeol();
                        move(y,x);
                    }
                    if(c2 == 10){
                        order[mistakeindex++]=order[currentcard];
                        attron(COLOR_PAIR(3));
                        mvprintw(LINES-3, (COLS-16-strlen(correctanswer))/2, "Correct answer: %s", correctanswer);
                        attron(COLOR_PAIR(1));

                    }
                    else {
                        box(form_win, 0, 0);
                        printProgress(form_win, currentcard, numCards);
                        werase(text);
                        wbkgd(text, COLOR_PAIR(2));
                        wprintw(text, "%s", currentFlashcard.definition);
                        wrefresh(form_win);
                        break;
                    }

                    form_driver(Form, REQ_DEL_CHAR);
                    
                    werase(result_win);
                }
                else {
                    curs_set(0);
                    //wprintctrx(result_win, 5, cols+4, "");
                    attron(COLOR_PAIR(9));
                    mvprintw(LINES-3, (COLS-7)/2, "Correct");
                    attron(COLOR_PAIR(1));
                    refresh();
                }


                currentcard++;
                if(currentcard>=numCards){
                    if(mistakeindex>0){
                        curs_set(0);
                        WINDOW* coverWindow = create_newwin(13, cols+4,(LINES - 11)/2,(COLS - cols-2)/2); wbkgd(coverWindow, COLOR_PAIR(1)); wrefresh(coverWindow);

                        WINDOW *ask_review = create_newwin(8,22, (LINES-6)/2, (COLS-20)/2); wbkgd(ask_review, COLOR_PAIR(2));
                        box(ask_review, 0, 0);
                        wprintctrx(ask_review, 1, 22, "quiz complete!");

                        bool currentselect=0;
                        WINDOW *review =     derwin(ask_review, 4, 10, 3, 1);     wbkgd(review, COLOR_PAIR(3));
                        WINDOW *return_win = derwin(ask_review, 4, 10, 3, 11);

                        mvwprintw(review, 1, 1, " review\n mistakes");
                        mvwprintw(return_win, 1, 1, " return \n to menu");
                        box(review,0,0);
                        box(return_win, 0, 0);

                        wrefresh(ask_review);

                        int ch = getch();
                        while(ch != 10){
                            switch (ch) {
                                case 'h':
                                    currentselect = 0;
                                    wbkgd(review, COLOR_PAIR(3)); wbkgd(return_win, COLOR_PAIR(2));
                                    break;
                                case 'l':
                                    currentselect = 1;
                                    wbkgd(review, COLOR_PAIR(2)); wbkgd(return_win, COLOR_PAIR(3));
                                    break;

                            }
                            touchwin(ask_review);
                            wrefresh(ask_review);
                            ch=getch();
                            

                        }

                    
                        erasewindow(coverWindow);
                        erasewindow(ask_review);
                        if(currentselect){
                            unpost_form(Form);
                            free_form(Form);
                            free_field(FileNameField[0]);
                            wborder(form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
                            werase(form_win);
                            delwin(form_win);
                            refresh();
                            //return NULL;
                            // update flashcard and form
                            return;
                        }
                        else{
                            currentcard = 0;
                            numCards = mistakeindex;
                            mistakeindex = 0;
                        }
                        touchwin(form_win);
                    }
                    else{
                        curs_set(0);
                        WINDOW* coverWindow = create_newwin(13, cols+4,(LINES - 11)/2,(COLS - cols-2)/2);
                        wbkgd(coverWindow, COLOR_PAIR(1));
                        wrefresh(coverWindow);

                        WINDOW *ask_review = create_newwin(8,22, (LINES-6)/2, (COLS-20)/2);
                        wbkgd(ask_review, COLOR_PAIR(2));
                        box(ask_review, 0, 0);
                        wprintctrx(ask_review, 1, 22, "quiz complete!");

                        WINDOW *return_win = derwin(ask_review, 4, 20, 3, 1);
                        wbkgd(return_win, COLOR_PAIR(3));

                        box(return_win, 0, 0);

                        wprintctrx(return_win, 1, 20, "return to menu");
                        wrefresh(ask_review);

                        int ch = getch();
                        unpost_form(Form);
                        free_form(Form);
                        free_field(FileNameField[0]);
                        erasewindow(form_win);
                        refresh();
                        //return NULL;
                        // update flashcard and form
                        return;
                    }
                    // say complete
                }


                box(form_win, 0, 0);
                printProgress(form_win, currentcard, numCards);
                werase(text);
                wbkgd(text, COLOR_PAIR(2));
                wprintw(text, "%s", currentFlashcard.definition);
                wrefresh(form_win);
                break;
            case 27:

                curs_set(0);
                unpost_form(Form);
                free_form(Form);
                free_field(FileNameField[0]);
                erasewindow(form_win);
                refresh();
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
                curs_set(1);
                form_driver(Form, ch);
                break;
        }
        wrefresh(form_win);
    }
    curs_set(0);

    unpost_form(Form);
    free_form(Form);
    free_field(FileNameField[0]);
    wborder(form_win, ' ',' ',' ',' ',' ',' ',' ',' ');
    wbkgd(form_win, COLOR_PAIR(1));
    wrefresh(form_win);
    delwin(form_win);
    refresh();

    return ;
}




