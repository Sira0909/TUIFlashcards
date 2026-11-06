//TODO: this doesnt react to KEY_RESIZE
//TODO: cross out selected in match
//TODO: hide other answers when one is selected for mc
//TODO: at least 1d of vectors
//TODO: multiple definitions
//TODO: animations and figure out list keybinds


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
#define spacing 5
static void ensureNotEqual(int* a, int numCards, int b, int c, int d){
    while (*a == b || *a==c ||*a==d){
        *a = rand()%numCards;
    }
}

void test(FlashcardSet *flashcard_set){
    // test will allways be shuffled and multivector, no settings menu needed
    int order[flashcard_set->num_items];

#ifdef _WIN32
    if(!getConfirmation("Hello! This study mode is currently incomplete on windows. Continue anyway?", NULL, "Returning.")){
        return;
    }
#endif
    


    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, 1, 0);
    if(numCards<8){
        showmsg("not enough cards");
        return;
    }

    WINDOW* mainquiz = create_newwin( LINES-2, 52, 1, (COLS-50)/2);
    wbkgd(mainquiz, COLOR_PAIR(2));
    WINDOW* numberwin = derwin(mainquiz, LINES-4, 3,  1, 2);
    WINDOW* qstionwin = derwin(mainquiz, LINES-4, 21, 1, 5);
    WINDOW* formwin =   derwin(mainquiz, LINES-4, 25, 1, 26);
    WINDOW* choicewin = derwin(mainquiz, LINES-4, 20, 1, 29);




    keypad(mainquiz, TRUE);

    int perSect = numCards/4;


    
    FIELD *nfield[numCards+2]; //numbers
    FIELD *rfield[numCards+2]; //response
    FIELD *qfield[numCards+2]; //question
    FIELD *cfield[numCards+2]; //answer choices
    for(int i = 0; i<numCards+2; i++){
        nfield[i]=NULL;rfield[i]=NULL;qfield[i]=NULL;cfield[i]=NULL;
    }
    FORM *numbers,*response,*question,*choices;
    curs_set(1);

    int perPage = min((LINES-4)/spacing,26);
    int pages= 0;

    char num[3] = "a.";
    
    //
    //multiple choice
    //
    char headers[4*((perSect+perPage-1)/perPage)][30];

    for(int i = 0; i < perSect; i++){
        nfield[i] = new_field(1, 3, spacing*(i%perPage), 0, 2, 1);
        rfield[i] = new_field(1, 1, spacing*(i%perPage), 1, 0, 1);
        qfield[i] = new_field(spacing-1, 20, spacing*(i%perPage), 0, MAX_FLASHCARD_SET_ITEM_SIZE/15, 0);
        cfield[i] = new_field(spacing, 20, spacing*(i%perPage), 0, MAX_FLASHCARD_SET_DEFN_SIZE/15, 0);
        set_field_back(rfield[i], COLOR_PAIR(3) | A_UNDERLINE);
        set_field_back(qfield[i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(nfield[i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(cfield[i], COLOR_PAIR(2));
        field_opts_on(rfield[i], O_AUTOSKIP);
        field_opts_on(cfield[i], O_WRAP);
        field_opts_on(qfield[i], O_WRAP);
        set_field_type(rfield[i], TYPE_ALPHA, 0);
        if (i!=0 && i%perPage==0){
            set_new_page(rfield[i], TRUE);
            set_new_page(qfield[i], TRUE);
            set_new_page(nfield[i], TRUE);
            set_new_page(cfield[i], TRUE);
            strcpy(headers[pages],"Match term to definition");
            pages++;
        }
    }
    strcpy(headers[pages],"Match term to definition");
    pages++;

    for(int page = 0; page <= pages; page++){
        int answers[perPage];
        int numchoices = perPage;
        if( page == pages){
            // account for final page being shorter
            numchoices = ((perSect-1)%perPage)+1;
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
        num[0]='a';
        for(int i = 0; i < numchoices; i++){
            char ans[2] = {(char)('a'+answers[i]), 0};
            set_field_buffer(rfield[page*perPage+i], 1, ans);
            //TODO: add defns
            set_field_buffer(cfield[page*perPage+i], 0, flashcard_set->cards[order[page*perPage+answers[i]]].definition[0]);
            set_field_buffer(qfield[page*perPage+i], 0, flashcard_set->cards[order[page*perPage+i]].term);
            set_field_buffer(nfield[page*perPage+i], 0, num);
            num[0]=num[0]+1;
        }
    }
    //
    // unscramble
    //
    for(int i = 0; i < perSect; i++){
        char* answerstr = flashcard_set->cards[order[perSect+i]].term;
        char questionstr[MAX_FLASHCARD_SET_ITEM_SIZE];
        strcpy(questionstr, answerstr);

        shufflePreserveGraphemes(questionstr);

        rfield[perSect+i] = new_field(1, 20, spacing*(i%perPage), 1, stringlen(answerstr)/20, 1);
        qfield[perSect+i] = new_field(1, 20, spacing*(i%perPage), 1, stringlen(answerstr)/20, 1);
        cfield[perSect+i] = new_field(1, 1, spacing*(i%perPage), 0, 0, 0); 
        nfield[perSect+i] = new_field(1, 3, spacing*(i%perPage), 0, 0, 0); 
        set_field_back(rfield[perSect+i], COLOR_PAIR(3) | A_UNDERLINE);
        set_field_back(nfield[perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(qfield[perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(cfield[perSect+i], COLOR_PAIR(2));
        field_opts_off(cfield[perSect+i], O_VISIBLE);
        set_field_buffer(rfield[perSect+i], 1, answerstr);
        set_field_buffer(qfield[perSect+i], 0, questionstr);
        set_field_buffer(nfield[perSect+i], 0, num);
        num[0]=(num[0]=='z')?'a':num[0]+1;
        //set_field_type(field[i], TYPE_ALPHA, 0);
        if (i%perPage==0){
            set_new_page(rfield[perSect+i], TRUE);
            set_new_page(qfield[perSect+i], TRUE);
            set_new_page(cfield[perSect+i], TRUE);
            set_new_page(nfield[perSect+i], TRUE);
            strcpy(headers[pages],"Unscramble the term");
            pages++;
        }
    }
    //
    // Multiple Choice
    //
    for(int i = 0; i < perSect; i++){
        char* term = flashcard_set->cards[order[perSect*2+i]].term;
        nfield[2*perSect+i] = new_field(1, 3, spacing*(i%perPage), 0, 2, 1);
        rfield[2*perSect+i] = new_field(1, 1, spacing*(i%perPage), 1, 0, 1);
        qfield[2*perSect+i] = new_field(1, 15, spacing*(i%perPage), 0, MAX_FLASHCARD_SET_ITEM_SIZE/15, 0);
#define size 20
        cfield[2*perSect+i] = new_field(4, size, spacing*(i%perPage), 0, 0, 0);
        set_field_back(rfield[2*perSect+i], COLOR_PAIR(3) | A_UNDERLINE);
        set_field_back(qfield[2*perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(nfield[2*perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(cfield[2*perSect+i], COLOR_PAIR(2));
        field_opts_on(rfield[2*perSect+i], O_AUTOSKIP);
        //field_opts_off(qfield[i], O_EDIT);
        //set_field_type(field[i], TYPE_ALPHA, 0);
        set_field_buffer(qfield[2*perSect+i], 0, term);
        set_field_buffer(nfield[2*perSect+i], 0, num);

        //TODO multiple sides
        int op[4] = {order[perSect*2+i], rand()%numCards, rand()%numCards, rand()%numCards};
        ensureNotEqual(&op[1], numCards, op[0], -1, -1);
        ensureNotEqual(&op[2], numCards, op[0], op[1], -1);
        ensureNotEqual(&op[3], numCards, op[0], op[1], op[2]);
        int correctans = rand()%4;
        int placeholder=op[0];
        op[0]=op[correctans];
        op[correctans]=placeholder;
        char answer[2]={'a'+correctans,0};
        set_field_buffer(rfield[2*perSect+i], 1, answer);

        //set up choices
        char buffer[size*4*2]; //*2 is to account for possible accents
        memset(buffer, ' ', size*4-1);
        int offset = 0;
        for(int j =0; j<4;j++){
            buffer[offset++]='a'+j;
            buffer[offset++]=')';
            offset++;
            int graphenes=0;
            for(int k=0; k-graphenes<size-3;k++){
                char* str =flashcard_set->cards[op[j]].definition[0];
                if(str[k]==0){
                    offset+=size-3+graphenes-k;
                    break;
                }
                if(str[k]==195-256){
                    graphenes++;
                }
                buffer[offset++]=str[k];
            }
        }
        buffer[size*4-1]='\0';

        set_field_buffer(cfield[2*perSect+i], 0, buffer);

        field_buffer(rfield[2*perSect+i], 1)[0]='0'+correctans;
        num[0]=(num[0]=='z')?'a':num[0]+1;
        if (i%perPage==0){
            set_new_page(rfield[2*perSect+i], TRUE);
            set_new_page(qfield[2*perSect+i], TRUE);
            set_new_page(nfield[2*perSect+i], TRUE);
            set_new_page(cfield[2*perSect+i], TRUE);
            strcpy(headers[pages],"Choose the correct definition");
            pages++;
        }
    }

    //
    //type
    //
    for(int i = 0; i < perSect; i++){
        char* questionstr = flashcard_set->cards[order[3*perSect+i]].term;
        char* answerstr = flashcard_set->cards[order[3*perSect+i]].definition[0];

        rfield[3*perSect+i] = new_field(1, 20, spacing*(i%perPage), 1, 1+stringlen(answerstr)/20, 1);
        qfield[3*perSect+i] = new_field(4, 20, spacing*(i%perPage), 1, stringlen(questionstr)/20, 1);
        cfield[3*perSect+i] = new_field(1, 1, spacing*(i%perPage), 0, 0, 0); 
        nfield[3*perSect+i] = new_field(1, 3, spacing*(i%perPage), 0, 0, 0); 
        set_field_back(rfield[3*perSect+i], COLOR_PAIR(3) | A_UNDERLINE);
        set_field_back(nfield[3*perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(qfield[3*perSect+i], COLOR_PAIR(2) | A_BOLD);
        set_field_back(cfield[3*perSect+i], COLOR_PAIR(2));
        field_opts_off(cfield[3*perSect+i], O_VISIBLE);
        set_field_buffer(rfield[3*perSect+i], 1, answerstr);
        set_field_buffer(qfield[3*perSect+i], 0, questionstr);
        set_field_buffer(nfield[3*perSect+i], 0, num);
        num[0]=(num[0]=='z')?'a':num[0]+1;
        //set_field_type(field[i], TYPE_ALPHA, 0);
        if (i%perPage==0){
            set_new_page(rfield[3*perSect+i], TRUE);
            set_new_page(qfield[3*perSect+i], TRUE);
            set_new_page(cfield[3*perSect+i], TRUE);
            set_new_page(nfield[3*perSect+i], TRUE);
            strcpy(headers[pages],"Type the definition");
            pages++;
        }
    }
    rfield[4*perSect] = NULL;
    qfield[4*perSect] = NULL;
    cfield[4*perSect] = NULL;
    nfield[4*perSect] = NULL;
    //set_new_page(rfield[4*perSect], TRUE);
    //set_new_page(qfield[4*perSect], TRUE);
    //set_new_page(cfield[4*perSect], TRUE);
    //set_new_page(nfield[4*perSect], TRUE);
    //rfield[4*perSect+1] = NULL;
    //qfield[4*perSect+1] = NULL;
    //cfield[4*perSect+1] = NULL;
    //nfield[4*perSect+1] = NULL;


    response = new_form(rfield);
    numbers = new_form(nfield);
    question = new_form(qfield);
    choices = new_form(cfield);

    box(mainquiz, 0, 0);
    set_form_win(numbers, mainquiz);
    set_form_sub(numbers, numberwin);
    post_form(numbers);

    set_form_win(question, mainquiz);
    set_form_sub(question, qstionwin);
    post_form(question);

    set_form_win(response, mainquiz);
    set_form_sub(response, formwin);
    post_form(response);


    set_form_win(choices, mainquiz);
    set_form_sub(choices, choicewin);
    post_form(choices);

    {
        box(mainquiz,0,0);
        printHeader(mainquiz, headers[0]);

    }
    

    //int basefield=0;

    int ch;
    wmove(mainquiz, 1,27);
    wrefresh(mainquiz);

    //for accents
    bool wasJustBacktick = false;
    bool wasJustTilde = false;
    bool submitted = 0;


    while((ch = wgetch(mainquiz)))
    {	

        switch(ch){
                    case 10:
                            if(!submitted) break;
                            set_field_buffer(current_field(response),0,field_buffer(current_field(response), 1));
                            set_field_fore(current_field(response), A_ITALIC);
                            break;

                    case 258:
                            /* Go to next field */
                            form_driver(response, REQ_NEXT_FIELD);
                            // /* Go to the end of the present buffer */
                            if(field_index(current_field(response))%perSect==0){
                                if(form_page(numbers)+1>=pages){
                                    curs_set(0);
                                    if(submitted&&getConfirmation("Return?", NULL, NULL)){
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
                                                free_field(cfield[i]);
                                                free_field(nfield[i]);
                                            }
                                        }
                                        erasewindow(formwin);
                                        erasewindow(choicewin);
                                        erasewindow(qstionwin);
                                        erasewindow(numberwin);
                                        erasewindow(mainquiz);
                                        curs_set(0);
                                        refresh();

                                        return;

                                    }
                                    else if(getConfirmation("Submit form?", NULL, NULL)){
                                        submitted = 1;
                                        int score = 0;
                                        for(int i = 0 ; i < 4*perSect;i++){
                                            if(strcmp(field_buffer(rfield[i],1),field_buffer(rfield[i], 0))==0){
                                                score++;
                                                set_field_fore(rfield[i], COLOR_PAIR(9));
                                            }
                                            else{
                                                set_field_fore(rfield[i], COLOR_PAIR(10));
                                            }
                                            field_opts_off(rfield[i], O_EDIT);
                                        }
                                        char scoreMsg[72];
                                        sprintf(scoreMsg, "You scored a %d/%d. Press enter to reveal correct answers.", score, 4*perSect); 
                                        showmsg(scoreMsg);
                                    }
                                    curs_set(1);
                                }
                                form_driver(numbers, REQ_NEXT_PAGE);
                                form_driver(question, REQ_NEXT_PAGE);
                                form_driver(response, REQ_NEXT_PAGE);
                                form_driver(choices, REQ_NEXT_PAGE);
                                if (!(field_opts(current_field(choices))&O_VISIBLE)){
                                    form_driver(response, REQ_PREV_PAGE);
                                    form_driver(response, REQ_NEXT_PAGE);
                                }
                                {
                                    int i = form_page(numbers);
                                    box(mainquiz,0,0);
                                    printHeader(mainquiz, headers[i]);

                                }
                                wmove(mainquiz, 1,27);
                            }
                            break;
                    case 259:
                            if(field_index(current_field(response))!=0 && field_index(current_field(response))%perSect==0){
                                form_driver(numbers, REQ_PREV_PAGE);
                                form_driver(question, REQ_PREV_PAGE);
                                form_driver(response, REQ_PREV_PAGE);
                                form_driver(choices, REQ_PREV_PAGE);
                                if (!(field_opts(current_field(choices))&O_VISIBLE)){
                                    form_driver(response, REQ_PREV_PAGE);
                                    form_driver(response, REQ_NEXT_PAGE);
                                }
                                {
                                    int i = form_page(numbers);
                                    box(mainquiz,0,0);
                                    printHeader(mainquiz, headers[i]);

                                }
                                wmove(mainquiz, 1,27);
                            }
                            /* Go to previous field */
                            form_driver(response, REQ_PREV_FIELD);
                            break;
                    case 261:
                            form_driver(response,REQ_NEXT_CHAR);
                            break;
                    case 260:
                            form_driver(response,REQ_PREV_CHAR);
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
                                    free_field(cfield[i]);
                                    free_field(nfield[i]);
                                }
                            }
                            erasewindow(formwin);
                            erasewindow(choicewin);
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
