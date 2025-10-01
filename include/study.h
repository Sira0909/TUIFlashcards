#include <flashcards.h>

void pickMode(char* list);

int getOrder(FlashcardSet *flashcard_set, int order[], bool shuffle, bool star_only);

void flashcard(FlashcardSet *flashcard_set, bool starred_only, bool shuffle);

void type(FlashcardSet *flashcard_set, bool starred_only, bool shuffle);
void multipleChoice(FlashcardSet *flashcard_set, bool starred_only, bool shuffle);
