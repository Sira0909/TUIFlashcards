#include "flashcards.h"
#include <string.h>
#include <stdlib.h>
#include <study.h>
#include <windows/window.h>
#include <ncurses.h>
#include <UI.h>
#include <config.h>

//TODO: fix this spaghetti
//TODO: does not support more questions than terms


struct MultipleChoice_metadata{
    int question_count;
    bool starred_only;
    bool shuffle;
    bool (*vectorsin);// ___->term
    bool (*vectorsout);// term->____

    //window info
    int maxlength;
    WINDOW* responseWindow;
    WINDOW* text;
    WINDOW* ans[2][2];
    WINDOW* ansBox[2][2];


    //state
    int selectedx;
    int selectedy;
    int currentcard;
    int correctans;


    FlashcardSet *flashcard_set;
    int *order;
    int *sides;
    char* question;
    char* choices[4];
};
#define Metadata ((struct MultipleChoice_metadata*)(metadata))

#define setallbkgd(x) \
{ \
    wbkgd(Metadata->responseWindow, COLOR_PAIR(x));\
    wbkgd(Metadata->ans[0][0], COLOR_PAIR(x));\
    wbkgd(Metadata->ans[0][1], COLOR_PAIR(x));\
    wbkgd(Metadata->ans[1][0], COLOR_PAIR(x));\
    wbkgd(Metadata->ans[1][1], COLOR_PAIR(x));\
    wbkgd(Metadata->ansBox[0][0], COLOR_PAIR(x));\
    wbkgd(Metadata->ansBox[0][1], COLOR_PAIR(x));\
    wbkgd(Metadata->ansBox[1][0], COLOR_PAIR(x));\
    wbkgd(Metadata->ansBox[1][1], COLOR_PAIR(x));\
    wbkgd(Metadata->text, COLOR_PAIR(x));\
}
#define currentflashcard Metadata->flashcard_set->cards[Metadata->order[Metadata->currentcard]]
#define currentFlashcard flashcard_set->cards[order[currentcard]]
#define card(x) Metadata->flashcard_set->cards[Metadata->order[x]]

void ensureNotEqual(int* a, int numCards, int b, int c, int d){
    while (*a == b || *a==c ||*a==d){
        *a = rand()%numCards;
    }
}


int getquestion(void* metadata){
    int side = Metadata->sides[Metadata->currentcard];
    int numCards = Metadata->flashcard_set->num_items;
    Metadata->question =   (side<0)
                        ?currentflashcard.term
                        //TODO: add defns
                        :currentflashcard.definition[abs(side)-1];

    int op[4] = {Metadata->currentcard, rand()%numCards, rand()%numCards, rand()%numCards};
    ensureNotEqual(&op[1], numCards, op[0], -1, -1);
    ensureNotEqual(&op[2], numCards, op[0], op[1], -1);
    ensureNotEqual(&op[3], numCards, op[0], op[1], op[2]);
    for(int i = 0;i<4;i++){
        Metadata->choices[i]=    (side<0) 
                            ?card(op[i]).definition[abs(side)-1]
                            :card(op[i]).term;

    }

    int correctans = rand()%4;
    char* placeholder=Metadata->choices[0];
    Metadata->choices[0]=Metadata->choices[correctans];
    Metadata->choices[correctans]=placeholder;
    return correctans;
}
int renderMC(void* metadata){
        setallbkgd(2);
        wbkgd(Metadata->ans[Metadata->selectedx][Metadata->selectedy], COLOR_PAIR(3));
        wbkgd(Metadata->ansBox[Metadata->selectedx][Metadata->selectedy], COLOR_PAIR(3));
        touchwin(Metadata->responseWindow);
        wrefresh(Metadata->responseWindow);
        return 1;
}
int MultipleChoice_starcurrent(void* window);
int MultipleChoice_starprevious(void* window);
int MultipleChoice_down(void* metadata);
int MultipleChoice_up(void* metadata);
int MultipleChoice_left(void* metadata);
int MultipleChoice_right(void* metadata);
int MultipleChoice_select(void*metadata);

int MultipleChoice_resetQuestion(void* metadata){
    Metadata->correctans = getquestion(metadata);

    printProgress(Metadata->responseWindow, Metadata->currentcard, Metadata->question_count);

    werase(Metadata->text);
    wprintctr(Metadata->text, Metadata->maxlength/3-12, Metadata->maxlength, Metadata->question);

    for(int i = 0; i < 4; i++){
        werase(Metadata->ansBox[i/2][i%2]);
        box(Metadata->ans[i/2][i%2],0,0);

    }

    wbkgd(Metadata->ans[Metadata->selectedx][Metadata->selectedy], COLOR_PAIR(3));
    wbkgd(Metadata->ansBox[Metadata->selectedx][Metadata->selectedy], COLOR_PAIR(3));

    for(int i = 0; i < 4; i++){
        wprintctr(Metadata->ansBox[i/2][i%2], 3, Metadata->maxlength/2-2, Metadata->choices[i]);
    }

    touchwin(Metadata->responseWindow);
    wrefresh(Metadata->responseWindow);


    return 1;
}



bool MC_setup(struct MultipleChoice_metadata* metadata, FlashcardSet* flashcard_set){


    //
    // settings
    //

    Metadata->question_count=flashcard_set->num_items;
    Metadata->starred_only=false;
    Metadata->shuffle=false;
    Metadata->vectorsin=calloc(flashcard_set->num_columns-1,sizeof(bool));
    Metadata->vectorsout=calloc(flashcard_set->num_columns-1,sizeof(bool));

    for(int i = 0 ; i < flashcard_set->num_columns-1; i++){
        Metadata->vectorsin[i]=true;
        Metadata->vectorsout[i]=true;
    }

    if (!get_settings(flashcard_set, &(Metadata->starred_only), &(Metadata->shuffle), &(Metadata->question_count), (Metadata->vectorsin), Metadata->vectorsout)){
        free(Metadata->vectorsin);
        free(Metadata->vectorsout);
        return false;
    }

    bool validvectors = false;//anything set to be studied
    for(int i = 0 ; i < flashcard_set->num_columns-1; i++){
        if(Metadata->vectorsin[i] || Metadata->vectorsout[i]){
            validvectors = true;
            break;
        }
    }
    if (!validvectors){
        free(Metadata->vectorsin);
        free(Metadata->vectorsout);
        return false;
    }



    //
    //state
    //
    
    metadata->selectedx=0;
    metadata->selectedy=0;
    metadata->currentcard=0;
    metadata->correctans=0;


    metadata->flashcard_set=flashcard_set;
    metadata->order = calloc(flashcard_set->num_items,sizeof(int));

    int numCards = getOrder(flashcard_set, metadata->order, Metadata->shuffle, Metadata->starred_only);

    //no cards match criteria
    if (!numCards){
        free(Metadata->vectorsin);
        free(Metadata->vectorsout);
        free(Metadata->order);
        return false;
    }
    if (numCards < 4){
        showmsg("Not enough cards. 4 required.");
        free(Metadata->vectorsin);
        free(Metadata->vectorsout);
        free(Metadata->order);
        return false;
    }



    Metadata->question_count=min(Metadata->question_count, numCards);




    metadata->sides = calloc(Metadata->question_count, sizeof(int));
    for(int i = 0; i<Metadata->question_count; i++){
        do{
        metadata->sides[i]= rand()%(flashcard_set->num_columns-1)+1;//plus 1 eliminates 0
        metadata->sides[i]*=(rand()%2==0)?1:-1;
        } while (
                (metadata->sides[i]>0
                    &&Metadata->vectorsin [metadata->sides[i]-1]==false) ||
                (metadata->sides[i]<0
                    &&Metadata->vectorsout[-metadata->sides[i]-1]==false)
        );
    }
#define MIN_LEN 51
    metadata->maxlength = MIN_LEN;
    for(int i = 0; i<Metadata->question_count;i++){
        if (strlen(flashcard_set->cards[metadata->order[i]].term)> metadata->maxlength)
        {
            metadata->maxlength = 
                strlen(flashcard_set->cards[metadata->order[i]].term);
        }
        for(int j = 0; i<flashcard_set->num_columns;i++){
            if (strlen(flashcard_set->cards[metadata->order[i]].definition[j])>metadata->maxlength){
                metadata->maxlength = strlen(flashcard_set->cards[metadata->order[i]].definition[j]);
            }
        }
    }


    #define maxlength metadata->maxlength
    int boxHeight = maxlength/3;
    int ansBoxHeight = 5;
    int ansBoxWidth = maxlength/2;
    int text_height = boxHeight-2-(ansBoxHeight*2);
    metadata->responseWindow = create_newwin(maxlength/3, maxlength+2,(LINES - maxlength/4)/2,(COLS - maxlength)/2);
    metadata->text  = derwin(metadata->responseWindow, text_height, maxlength, 1, 1);
    metadata->ans[0][0] = derwin(metadata->responseWindow, ansBoxHeight, ansBoxWidth, (boxHeight)-(2*ansBoxHeight+1),  1               );
    metadata->ans[0][1] = derwin(metadata->responseWindow, ansBoxHeight, ansBoxWidth, (boxHeight)-(ansBoxHeight+1),   1               );
    metadata->ans[1][0] = derwin(metadata->responseWindow, ansBoxHeight, ansBoxWidth, (boxHeight)-(2*ansBoxHeight+1),  2+maxlength/2   );
    metadata->ans[1][1] = derwin(metadata->responseWindow, ansBoxHeight, ansBoxWidth, (boxHeight)-(ansBoxHeight+1),   2+maxlength/2   );
    metadata->ansBox[0][0] = derwin(metadata->responseWindow, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight*2,   2               );
    metadata->ansBox[0][1] = derwin(metadata->responseWindow, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight  ,   2               );
    metadata->ansBox[1][0] = derwin(metadata->responseWindow, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight*2,   3+maxlength/2   );
    metadata->ansBox[1][1] = derwin(metadata->responseWindow, ansBoxHeight-2, ansBoxWidth-2, (boxHeight)-ansBoxHeight  ,   3+maxlength/2   );
    #undef maxlength

    wbkgd(metadata->responseWindow, COLOR_PAIR(2));
    box(metadata->responseWindow, 0,0);
    printProgress(metadata->responseWindow, metadata->currentcard, Metadata->question_count);
    //int texty=(maxlength/3-11)/2;
    //wprintctrx(metadata->responseWindow, texty, maxlength+2, question);
    wrefresh(metadata->responseWindow);
    return true;


}

void multipleChoice(FlashcardSet *flashcard_set){
    bind_keys(multipleChoiceKeybinds, renderMC, 11)
        {config.keylayout.lkey,config.keylayout.str_lkey, "left",&MultipleChoice_left},
        {config.keylayout.dkey,config.keylayout.str_dkey, "down",&MultipleChoice_down},
        {config.keylayout.ukey,config.keylayout.str_ukey,   "up",&MultipleChoice_up},
        {config.keylayout.rkey,config.keylayout.str_rkey,"right",&MultipleChoice_right},
        {10,"<enter>", "select", &MultipleChoice_select},
        {'s', "s", "star current", &MultipleChoice_starcurrent},
        {'S', "S", "star previous", &MultipleChoice_starprevious},
        {27, " ", " ", &quit},
        {'q', "q / <esc>", " ", &quit},
        {-1, "?", "list keybinds", NULL}
    };
    struct MultipleChoice_metadata metadata;

    if(!MC_setup(&metadata, flashcard_set)){
        return;
    }


    





    MultipleChoice_resetQuestion(&metadata);

    run(&metadata, multipleChoiceKeybinds);
    free(metadata.order);
    free(metadata.sides);
    free(metadata.vectorsin);
    free(metadata.vectorsout);
    move(LINES-3, 0);
    clrtoeol();
    
    erasewindow(metadata.responseWindow);
}
int MultipleChoice_down(void* metadata){
    
    Metadata->selectedy = (Metadata->selectedy - 1);
    if (Metadata->selectedy < 0) Metadata->selectedy = 1;
    return 1;
}
int MultipleChoice_up(void* metadata){
    Metadata->selectedy = (Metadata->selectedy + 1);
    if (Metadata->selectedy > 1) Metadata->selectedy = 0;
    return 1;

}
int MultipleChoice_left(void* metadata){
    Metadata->selectedx = (Metadata->selectedx - 1);
    if (Metadata->selectedx < 0) Metadata->selectedx = 1;
    return 1;
}
int MultipleChoice_right(void* metadata){
    Metadata->selectedx = (Metadata->selectedx + 1);
    if (Metadata->selectedx > 1) Metadata->selectedx = 0;
    return 1;
}
int MultipleChoice_starprevious(void*metadata){
        move(LINES-3, 0);
        clrtoeol();
        if(Metadata->currentcard>0){
            attron(COLOR_PAIR(1));
            Metadata->flashcard_set->cards[Metadata->order[Metadata->currentcard-1]].is_starred = !Metadata->flashcard_set->cards[Metadata->order[Metadata->currentcard-1]].is_starred;
            wprintctrx(stdscr, LINES-3, COLS, (Metadata->flashcard_set->cards[Metadata->order[Metadata->currentcard-1]].is_starred) ? "Previous flashcard has been starred" : "Previous flashcard has been unstarred");
            attron(COLOR_PAIR(1));
            
        }
        else{
            attron(COLOR_PAIR(1));
            currentflashcard.is_starred = !currentflashcard.is_starred;
            wprintctrx(stdscr, LINES-3, COLS, (currentflashcard.is_starred) ? "Flashcard has been starred" : "Flashcard has been unstarred");
            attron(COLOR_PAIR(1));
        }
        return 1;
}
int MultipleChoice_starcurrent(void*metadata){
        move(LINES-3, 0);
        clrtoeol();
        attron(COLOR_PAIR(1));
        currentflashcard.is_starred = !currentflashcard.is_starred;
        wprintctrx(stdscr, LINES-3, COLS, (currentflashcard.is_starred) ? "Flashcard has been starred" : "Flashcard has been unstarred");
        attron(COLOR_PAIR(1));
        return 1;
}
int MultipleChoice_select(void*metadata){
    move(LINES-3, 0);
    clrtoeol();

    if(Metadata->selectedx*2+Metadata->selectedy != Metadata->correctans){ // incorrect
        //setallbkgd(10);
        attron(COLOR_PAIR(10));
        mvprintw(LINES-3, (COLS-16-strlen(Metadata->choices[Metadata->correctans]))/2, "Correct answer: %s", Metadata->choices[Metadata->correctans]);
        attron(COLOR_PAIR(1));
    }
    else {
        attron(COLOR_PAIR(9));
        mvprintw(LINES-3, (COLS-8)/2, "Correct!");
        attron(COLOR_PAIR(1));
    }


    Metadata->currentcard++;


    if(Metadata->currentcard>=Metadata->question_count){
        curs_set(0);
        WINDOW* coverWindow = create_newwin(Metadata->maxlength/3, Metadata->maxlength+2,(LINES - Metadata->maxlength/4)/2,(COLS - Metadata->maxlength)/2);
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
        erasewindow(Metadata->responseWindow);
        refresh();
        //return NULL;
        // update flashcard and form
        return -1;
    }
    MultipleChoice_resetQuestion(metadata);
    return 1;
}
