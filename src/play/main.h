#include <ncurses.h>
#include "../helpers.h"
#include "../window.h"
#include "../flashcards.h"
void play(char* list);


void flashcard(FlashcardSet *flashcard_set, bool starred_only, bool shuffle);

void type(FlashcardSet *flashcard_set);
