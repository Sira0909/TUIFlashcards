#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
#include <UI.h>

//TODO: fix this spaghetti

char multipleChoiceKeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};
#define setallbkgd(x) \
{ \
    wbkgd(response_win, COLOR_PAIR(x));\
    wbkgd(ans_1, COLOR_PAIR(x));\
    wbkgd(ans_2, COLOR_PAIR(x));\
    wbkgd(ans_3, COLOR_PAIR(x));\
    wbkgd(ans_4, COLOR_PAIR(x));\
    wbkgd(ansBox_1, COLOR_PAIR(x));\
    wbkgd(ansBox_2, COLOR_PAIR(x));\
    wbkgd(ansBox_3, COLOR_PAIR(x));\
    wbkgd(ansBox_4, COLOR_PAIR(x));\
    wbkgd(text, COLOR_PAIR(x));\
}
#define currentFlashcard flashcard_set->cards[order[currentcard]]

void ensureNotEqual(int* a, int numCards, int b, int c, int d){
    while (*a == b || *a==c ||*a==d){
        *a = rand()%numCards;
    }
}
int getquestion(FlashcardSet* flashcard_set, int currentcard, int numCards, int* order, int side, char** question, char** choice1,char** choice2,char** choice3,char** choice4){
    srand(time(0));
    *question =    (side) 
                        ?currentFlashcard.term
                        :currentFlashcard.definition;

    int op1 = currentcard;
    int op2 = rand()%numCards;
    int op3 = rand()%numCards;
    int op4 = rand()%numCards;
    ensureNotEqual(&op2, numCards, op1, -1, -1);
    ensureNotEqual(&op3, numCards, op1, op2, -1);
    ensureNotEqual(&op4, numCards, op1, op2, op3);
    *choice1 =    (side) 
                        ?flashcard_set->cards[order[op1]].definition
                        :flashcard_set->cards[order[op1]].term;
    *choice2 =    (side) 
                        ?flashcard_set->cards[order[op2]].definition
                        :flashcard_set->cards[order[op2]].term;
    *choice3 =    (side) 
                        ?flashcard_set->cards[order[op3]].definition
                        :flashcard_set->cards[order[op3]].term;
    *choice4=    (side) 
                        ?flashcard_set->cards[order[op4]].definition
                        :flashcard_set->cards[order[op4]].term;

    int correctans = rand()%4;
    char* placeholder=*choice1;
    switch(correctans){
        case 0:
            break;
        case 1:
            *choice1=*choice2;
            *choice2=placeholder;
            break;
        case 2:
            *choice1=*choice3;
            *choice3=placeholder;
            break;
        case 3:
            *choice1=*choice4;
            *choice4=placeholder;
            break;
    }
    return correctans;
}


void multipleChoice(FlashcardSet *flashcard_set){
    bool starred_only = 0;
    bool shuffle = 0;
    int vectors = 0;
    if (!get_settings(&starred_only, &shuffle,&vectors)){
        return;
    }

    int order[flashcard_set->num_items];
#define MIN_LEN 51
    int maxlength = MIN_LEN;



    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, shuffle, starred_only);

    if (!numCards)
        return;
    if (numCards < 4){
        WINDOW* errorWin = create_newwin(3, 30, (LINES-1)/2, (COLS-28)/2);
        wbkgd(errorWin, COLOR_PAIR(7));
        box(errorWin,0,0);
        wattron(errorWin,A_BOLD);
        mvwprintw(errorWin,1,1, "not enough cards. 4 required");
        wmove(errorWin, 0, 1);
        waddch(errorWin, ACS_RTEE);
        wprintw(errorWin, "%s", "Error");
        waddch(errorWin, ACS_LTEE);
        wrefresh(errorWin);
        getch();
        erasewindow(errorWin);
        return;
    }

    for(int i = 0; i<numCards;i++){
        if (strlen(flashcard_set->cards[order[i]].term)>maxlength){
            maxlength = strlen(flashcard_set->cards[i].term);
        }
        if (strlen(flashcard_set->cards[order[i]].definition)>maxlength){
            maxlength = strlen(flashcard_set->cards[i].definition);
        }
    }

    
    int currentcard = 0;
    int side = 0;

    if (vectors==1){
        side = 1;
    }
    if (vectors==2){
        side=rand()%2;
    }

    int selectedx = 0;
    int selectedy = 0;
    char* question;
    char* choice1;
    char* choice2;
    char* choice3;
    char* choice4;
    int correctans;

    WINDOW* response_win = create_newwin(maxlength/3, maxlength+2,(LINES - maxlength/4)/2,(COLS - maxlength)/2);
    correctans = getquestion(flashcard_set, currentcard, numCards, order, side, &question, &choice1, &choice2, &choice3, &choice4);

    wbkgd(response_win, COLOR_PAIR(2));
    box(response_win, 0,0);

    //int texty=(maxlength/3-11)/2;
    //wprintctrx(response_win, texty, maxlength+2, question);
    wrefresh(response_win);


    int ch;
    int boxHeight = maxlength/3;
    int ansBoxHeight = 5;
    int ansBoxWidth = maxlength/2;
    int text_height = boxHeight-2-(ansBoxHeight*2);

    WINDOW* text  = derwin(response_win, text_height, maxlength, 1, 1);
    WINDOW* ans_1 = derwin(response_win, ansBoxHeight, ansBoxWidth, (boxHeight)-(2*ansBoxHeight+1),  1               );
    WINDOW* ans_2 = derwin(response_win, ansBoxHeight, ansBoxWidth, (boxHeight)-(ansBoxHeight+1),   1               );
    WINDOW* ans_3 = derwin(response_win, ansBoxHeight, ansBoxWidth, (boxHeight)-(2*ansBoxHeight+1),  2+maxlength/2   );
    WINDOW* ans_4 = derwin(response_win, ansBoxHeight, ansBoxWidth, (boxHeight)-(ansBoxHeight+1),   2+maxlength/2   );
    WINDOW* ansBox_1 = derwin(response_win, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight*2,   2               );
    WINDOW* ansBox_2 = derwin(response_win, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight  ,   2               );
    WINDOW* ansBox_3 = derwin(response_win, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight*2,   3+maxlength/2   );
    WINDOW* ansBox_4 = derwin(response_win, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight  ,   3+maxlength/2   );
    WINDOW *ans[2][2] = { {ans_1, ans_3}, {ans_2, ans_4} };
    WINDOW *ansBox[2][2] = { {ansBox_1, ansBox_3}, {ansBox_2, ansBox_4} };






    werase(text);
    werase(ansBox_1);
    werase(ansBox_2);
    werase(ansBox_3);
    werase(ansBox_4);
    box(ans_1, 0,0);
    box(ans_2, 0,0);
    box(ans_3, 0,0);
    box(ans_4, 0,0);
    wbkgd(ans[selectedy][selectedx], COLOR_PAIR(3));
    wbkgd(ansBox[selectedy][selectedx], COLOR_PAIR(3));
    wprintctr(text, maxlength/3-12, maxlength, question);
    wprintctr(ansBox_1, 3, maxlength/2-2, choice1);
    wprintctr(ansBox_2, 3, maxlength/2-2, choice2);
    wprintctr(ansBox_3, 3, maxlength/2-2, choice3);
    wprintctr(ansBox_4, 3, maxlength/2-2, choice4);
    touchwin(response_win);
    
    wrefresh(response_win);
    
    
    while (1 == 1){
        ch = getch();
        move(LINES-3, 0);
        clrtoeol();
        switch(ch){
            case 'h': 
                selectedx = (selectedx - 1);
                if (selectedx < 0) selectedx = 1;
                break;
            case 'j':
                selectedy = (selectedy - 1);
                if (selectedy < 0) selectedy = 1;
                break;
            case 'k':
                selectedy = (selectedy + 1) % 2;
                if (selectedy > 1) selectedy = 0;
                break;
            case 'l':
                selectedx = (selectedx + 1) % 2;
                if (selectedx > 1) selectedx = 0;
                break;
            case 'S': //star previous
                if(currentcard>0){
                    attron(COLOR_PAIR(1));
                    flashcard_set->cards[order[currentcard-1]].is_starred = !flashcard_set->cards[order[currentcard-1]].is_starred;
                    wprintctrx(stdscr, LINES-3, COLS, (flashcard_set->cards[order[currentcard-1]].is_starred) ? "Previous flashcard has been starred" : "Previous flashcard has been unstarred");
                    attron(COLOR_PAIR(1));
                    break;
                }
                // stars current when this is first flashcard
            case 's': //star current
                attron(COLOR_PAIR(1));
                currentFlashcard.is_starred = !currentFlashcard.is_starred;
                wprintctrx(stdscr, LINES-3, COLS, (currentFlashcard.is_starred) ? "Flashcard has been starred" : "Flashcard has been unstarred");
                attron(COLOR_PAIR(1));
                break;
            case 27:
            case 'q':
                erasewindow(response_win);
                return;
                //maybe allow leave early
                break;
            case 10:
                {
                    move(LINES-3, 0);
                    clrtoeol();

                    if(selectedx*2+selectedy != correctans){ // incorrect
                        //setallbkgd(10);
                        attron(COLOR_PAIR(10));
                        switch(correctans){
                            case 0:
                                mvprintw(LINES-3, (COLS-16-strlen(choice1))/2, "Correct answer: %s", choice1);
                                break;
                            case 1:
                                mvprintw(LINES-3, (COLS-16-strlen(choice2))/2, "Correct answer: %s", choice2);
                                break;
                            case 2:
                                mvprintw(LINES-3, (COLS-16-strlen(choice3))/2, "Correct answer: %s", choice3);
                                break;
                            case 3:
                                mvprintw(LINES-3, (COLS-16-strlen(choice4))/2, "Correct answer: %s", choice4);
                                break;
                        }
                        attron(COLOR_PAIR(1));
                    }
                    else {
                        attron(COLOR_PAIR(9));
                        mvprintw(LINES-3, (COLS-8)/2, "Correct!");
                        attron(COLOR_PAIR(1));
                    }

                    touchwin(response_win);
                    
                    wrefresh(response_win);

                    currentcard++;


                    if(currentcard>=numCards){
                        curs_set(0);
                        WINDOW* coverWindow = create_newwin(maxlength/3, maxlength+2,(LINES - maxlength/4)/2,(COLS - maxlength)/2);
                        wbkgd(coverWindow, COLOR_PAIR(1));
                        wrefresh(coverWindow);

                        WINDOW *ask_review = create_newwin(8,22, (LINES-6)/2, (COLS-20)/2);
                        wbkgd(ask_review, COLOR_PAIR(2));
                        box(ask_review, 0, 0);
                        wprintctrx(ask_review, 1, 22, "Quiz complete!");

                        WINDOW *return_win = derwin(ask_review, 4, 20, 3, 1);
                        wbkgd(return_win, COLOR_PAIR(3));

                        box(return_win, 0, 0);

                        wprintctrx(return_win, 1, 20, "Return to menu");
                        wrefresh(ask_review);

                        getch();
                        erasewindow(response_win);
                        refresh();
                        //return NULL;
                        // update flashcard and form
                        return;
                    }
                    if (vectors==2){
                        side=rand()%2;
                    }
                    correctans = getquestion(flashcard_set, currentcard, numCards, order, side, &question, &choice1, &choice2, &choice3, &choice4);


                    werase(text);
                    werase(ansBox_1);
                    werase(ansBox_2);
                    werase(ansBox_3);
                    werase(ansBox_4);
                    box(ans_1, 0,0);
                    box(ans_2, 0,0);
                    box(ans_3, 0,0);
                    box(ans_4, 0,0);
                    wbkgd(ans[selectedy][selectedx], COLOR_PAIR(3));
                    wprintctr(text, maxlength/3-12, maxlength, question);
                    wprintctr(ansBox_1, 3, maxlength/2-2, choice1);
                    wprintctr(ansBox_2, 3, maxlength/2-2, choice2);
                    wprintctr(ansBox_3, 3, maxlength/2-2, choice3);
                    wprintctr(ansBox_4, 3, maxlength/2-2, choice4);
                    break;
                }
            case '?':
                list_keybinds(7, multipleChoiceKeybinds);
                touchwin(response_win);
                wrefresh(response_win);
                break;
        }
        if(ch !=10){
            setallbkgd(2);
        }
        wbkgd(ans[selectedy][selectedx], COLOR_PAIR(3));
        wbkgd(ansBox[selectedy][selectedx], COLOR_PAIR(3));
        touchwin(response_win);
        wrefresh(response_win);
    }
    getch();
    erasewindow(response_win);
}
