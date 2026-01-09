
//
// 
// UNFINISHED
// TODO: FINISH
//

// structure: 
//
// match section: definitions given a number, match term to definition
// unscramble: given terms in scrambled form, provide unscrambled word
// short multiple choice
// short type
//
#include <stdlib.h>

#include <study.h>
#include <ncurses.h>
#include <form.h>
#include <windows/window.h>
#include <UI.h>



void test(FlashcardSet *flashcard_set){
    return; // unfinished
    // test will allways be shuffled and multivector, no settings menu needed
    int order[flashcard_set->num_items];

#define MIN_LEN 51
    int maxlength = MIN_LEN;
    


    //no cards match criteria
    int numCards = getOrder(flashcard_set, order, 1, 0);

    int section = 0;


    WINDOW* mainquiz = create_newwin( LINES-2, 52, 1, (COLS-50)/2);
    wbkgd(mainquiz, COLOR_PAIR(2));
    WINDOW* formwin = derwin(mainquiz, LINES-4, 25, 1, 26);
    WINDOW* qstionwin = derwin(mainquiz, LINES-4, 20, 1, 5);
    WINDOW* answerwin = derwin(mainquiz, LINES-4, 20, 1, 30);


    keypad(mainquiz, TRUE);

    int perSect = numCards/4;


    
    FIELD *field[perSect+1];
    FORM *Match;
    curs_set(1);

    int perPage = min((LINES-4)/3,26);
    int pages= 0;
    for(int i = 0; i < perSect; i++){
        field[i] = new_field(1, 1, 3*(i%perPage), 1, 0, 1);
        set_field_back(field[i], COLOR_PAIR(3) | A_UNDERLINE);
        field_opts_on(field[i], O_AUTOSKIP);
        //set_field_type(field[i], TYPE_ALPHA, 0);
        if (i!=0 && i%perPage==0){
            set_new_page(field[i], TRUE);
            pages++;
        }
    }
    field[perSect] = NULL;

    Match = new_form(field);

    box(mainquiz, 0, 0);
    set_form_win(Match, mainquiz);
    set_form_sub(Match, formwin);

    post_form(Match);
    //wrefresh(mainquiz);

    for(int page = 0; page <= pages; page++){
        int numchoices = perPage;
        if( page == pages){
            // account for final page being shorter
            numchoices = perSect%perPage;
        }

        int answers[numchoices];
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
            const char ans[2] = {(char)('a'+answers[i]), 0};
            set_field_buffer(field[page*perPage+i], 1, ans);
        }
    }

    for(int page = 0; page <= pages; page++){
        int numchoices = perPage;
        if( page == pages){
            // account for final page being shorter
            numchoices = perSect%perPage;
        }
        for(int i = 0; i < numchoices; i++){
            wattron(mainquiz, A_BOLD);
            mvwprintw(mainquiz, 1+3*i, 2, "%c.",'a'+i);
            wattroff(mainquiz, A_BOLD);
            //mvwprintw(formwin, 3*i, 1, ".");
            mvwprintw(qstionwin, 3*i, 0, "%s", flashcard_set->cards[order[page*perPage+i]].definition);
           // mvwprintw(answerwin, 3*i, 0, "%s", flashcard_set->cards[order[page*perPage+ans[i]]].definition);

        }
        wmove(mainquiz, 1,27);
        //wrefresh(mainquiz);
        int ch;
        while((ch = wgetch(mainquiz)))
	{	switch(ch)
		{	case 258:
				/* Go to next field */
				form_driver(Match, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				break;
			case 259:
				/* Go to previous field */
				form_driver(Match, REQ_PREV_FIELD);
				break;
			default:
				/* If this is a normal character, it gets */
				/* Printed				  */	
                                if (('a'<=ch && ch <='z') || ('A'<=ch && ch <='Z')){
                                    form_driver(Match, ch);
                                }
				break;
		}
	}

        getch();
    }
    




}
