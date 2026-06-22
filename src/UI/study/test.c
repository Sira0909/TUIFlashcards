
//
// 
// UNFINISHED AND VERY BROKEN
// TODO: FINISH
//

// structure: 
//
// match section: definitions given a number, match term to definition
// unscramble: given terms in scrambled form, provide unscrambled word
// short multiple choice
// short type
//
#include "flashcards.h"
#include <config.h>
#include <macros.h>
#include <stdio.h>
#include <stdlib.h>

#include <study.h>
#include <string.h>

#define _XOPEN_SOURCE 600
#include <ncurses.h>
#include <form.h>
#include <windows/window.h>
#include <UI.h>

#define MIN_LEN 51
#define spacing 4


void test(FlashcardSet *flashcard_set){
    return;// not really usable yet. delete this if you want
    //return; // unfinished
    // test will allways be shuffled and multivector, no settings menu needed
    int order[flashcard_set->num_items];

//    int maxlength = MIN_LEN;
    


    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, 1, 0);

    WINDOW* mainquiz = create_newwin( LINES-2, 52, 1, (COLS-50)/2);
    wbkgd(mainquiz, COLOR_PAIR(2));
    WINDOW* formwin = derwin(mainquiz, LINES-4, 25, 1, 26);
    WINDOW* qstionwin = derwin(mainquiz, LINES-4, 21, 1, 5);
    WINDOW* numberwin = derwin(mainquiz, LINES-4, 3, 1, 2);
//    wbkgd(formwin, COLOR_PAIR(3));
    //wbkgd(qstionwin, COLOR_PAIR(3));


    WINDOW* answerwin = derwin(mainquiz, LINES-4, 20, 1, 30);


    keypad(mainquiz, TRUE);

    int perSect = numCards/4;


    
    FIELD *nfield[numCards+2]; //numbers
    FIELD *rfield[numCards+2]; //response
    FIELD *qfield[numCards+2]; //question
    FIELD *afield[numCards+2]; //answer choices
    for(int i = 0; i<numCards+2; i++){
        nfield[i]=NULL;
        rfield[i]=NULL;
        qfield[i]=NULL;
        afield[i]=NULL;
    }
    FORM *numbers;
    FORM *response;
    FORM *question;
    FORM *choices;
    curs_set(1);

    int perPage = min((LINES-4)/spacing,26);
    int pages= 0;
    char num[3] = "a.";
    //
    //multiple choice
    //

    for(int i = 0; i < perSect; i++){
        nfield[i] = new_field(1, 3, spacing*(i%perPage), 0, 2, 1);
        rfield[i] = new_field(1, 1, spacing*(i%perPage), 1, 0, 1);
        qfield[i] = new_field(1, 15, spacing*(i%perPage), 0, MAX_FLASHCARD_SET_ITEM_SIZE/15, 0);
        afield[i] = new_field(1, 15, spacing*(i%perPage), 0, MAX_FLASHCARD_SET_DEFN_SIZE/15, 0);
        if(afield[i]==NULL){
            endwin();
            perror("error:");
        }
        set_field_back(rfield[i], COLOR_PAIR(3) | A_UNDERLINE);
        set_field_back(qfield[i], COLOR_PAIR(2));
        set_field_back(nfield[i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(afield[i], COLOR_PAIR(2));
        field_opts_on(rfield[i], O_AUTOSKIP);
        //field_opts_off(qfield[i], O_EDIT);
        //set_field_type(field[i], TYPE_ALPHA, 0);
        if (i!=0 && i%perPage==0){
            set_new_page(rfield[i], TRUE);
            set_new_page(qfield[i], TRUE);
            set_new_page(nfield[i], TRUE);
            set_new_page(afield[i], TRUE);
            pages++;
        }
    }

    for(int page = 0; page <= pages; page++){
        int answers[perPage];
        int numchoices = perPage;
        if( page == pages){
            // account for final page being shorter
            numchoices = perSect%perPage;
        }

        for(int i = 0; i < numchoices; i++){
            answers[i]=i;
        }

        // shuffle answers
        for(int i = 0; i < numchoices; i++){
            int toswap = rand()% numchoices;
            int tmp = answers[i];
            answers[i] = answers[toswap];
            answers[toswap] = tmp;
        }
        for(int i = 0; i < numchoices; i++){
            char ans[2] = {(char)('a'+answers[i]), 0};
            //set_field_buffer(rfield[page*perPage+i], 1, ans);
            //TODO: add defns
            set_field_buffer(afield[page*perPage+i], 0, flashcard_set->cards[order[page*perPage+answers[i]]].definition[0]);
            set_field_buffer(qfield[page*perPage+i], 0, flashcard_set->cards[order[page*perPage+i]].term);
            set_field_buffer(nfield[page*perPage+i], 0, num);
            num[0]=(num[0]=='z')?'a':num[0]+1;
        }
    }
    //
    // unscramble
    //
    for(int i = 0; i < perSect; i++){
        char* answerstr = flashcard_set->cards[order[perSect+i]].term;
        char questionstr[MAX_FLASHCARD_SET_ITEM_SIZE];
        strcpy(questionstr, answerstr);

        //shufflePreserveGraphemes(questionstr);

        //rfield[perSect+i] = new_field(1, 20, spacing*(i%perPage), 1, strlen(answerstr)/20, 1);
        //qfield[perSect+i] = new_field(1, 20, spacing*(i%perPage), 1, strlen(answerstr)/20, 1);
        //afield[perSect+i] = new_field(1, 1, spacing*(i%perPage), 0, 0, 0); 
        //nfield[perSect+i] = new_field(1, 3, spacing*(i%perPage), 0, 0, 0); 
        //set_field_back(rfield[perSect+i], COLOR_PAIR(3) | A_UNDERLINE);
        //set_field_back(nfield[perSect+i], COLOR_PAIR(2) | A_BOLD);
        //set_field_back(qfield[perSect+i], COLOR_PAIR(2));
        //set_field_back(afield[perSect+i], COLOR_PAIR(2));
        //field_opts_off(afield[perSect+i], O_VISIBLE);
        //set_field_buffer(rfield[perSect+i], 1, answerstr);
        //set_field_buffer(qfield[perSect+i], 0, questionstr);
        //set_field_buffer(nfield[perSect+i], 0, num);
        num[0]=(num[0]=='z')?'a':num[0]+1;
        //set_field_type(field[i], TYPE_ALPHA, 0);
        if (i%perPage==0){
//            set_new_page(rfield[perSect+i], TRUE);
            //set_new_page(qfield[perSect+i], TRUE);
            //set_new_page(afield[perSect+i], TRUE);
            //set_new_page(nfield[perSect+i], TRUE);
        }
    }
    rfield[2*perSect] = NULL;
    qfield[2*perSect] = NULL;
    afield[2*perSect] = NULL;
    nfield[2*perSect] = NULL;


    response = new_form(rfield);
    numbers = new_form(nfield);
    question = new_form(qfield);
    choices = new_form(afield);

    box(mainquiz, 0, 0);
    set_form_win(numbers, mainquiz);
    set_form_sub(numbers, numberwin);
    post_form(numbers);

    set_form_win(response, mainquiz);
    set_form_sub(response, formwin);
    post_form(response);

    set_form_win(question, mainquiz);
    set_form_sub(question, qstionwin);
    post_form(question);

    set_form_win(choices, mainquiz);
    set_form_sub(choices, answerwin);
    post_form(choices);

    

    int basefield=0;

    int ch;
    wmove(mainquiz, 1,27);
    wrefresh(mainquiz);

    //for accents
    bool wasJustBacktick = false;
    bool wasJustTilde = false;


    while((ch = wgetch(mainquiz)))
    {	

        switch(ch){
                    case 258:
                            /* Go to next field */
                            form_driver(response, REQ_NEXT_FIELD);
                            /* Go to the end of the present buffer */
                            break;
                    case 259:
                            /* Go to previous field */
                            form_driver(response, REQ_PREV_FIELD);
                            break;
                    case 261:
                            form_driver(question, REQ_NEXT_PAGE);
                            form_driver(choices, REQ_NEXT_PAGE);
                            form_driver(response, REQ_NEXT_PAGE);
                            form_driver(numbers, REQ_NEXT_PAGE);
                            break;
                    case 260:
                            form_driver(question, REQ_PREV_PAGE);
                            form_driver(choices, REQ_PREV_PAGE);
                            form_driver(response, REQ_PREV_PAGE);
                            form_driver(numbers, REQ_PREV_PAGE);
                            break;
                    case KEY_BACKSPACE:
                    case 127:
                        form_driver(response, REQ_DEL_PREV);
                        break;
                    case 27:
                            unpost_form(question);
                            unpost_form(response);
                            unpost_form(choices);
                            unpost_form(numbers);
                            free_form(question);
                            free_form(response);
                            free_form(choices);
                            free_form(numbers);
                            for(int i = 0; i < perSect*2; i++){
                                if(rfield[i]!=NULL){
                                    free_field(rfield[i]);
                                    free_field(qfield[i]);
                                    free_field(afield[i]);
                                    free_field(nfield[i]);
                                }
                            }
                            erasewindow(formwin);
                            erasewindow(answerwin);
                            erasewindow(qstionwin);
                            erasewindow(numberwin);
                            erasewindow(mainquiz);
                            curs_set(0);
                            refresh();

                            return;
                    handleAccents(response)
            }
    }
}
