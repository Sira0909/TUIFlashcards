#include <flashcards.h>

bool get_settings(bool* starred_only, bool* shuffle);

void pickMode(char* list);

int getOrder(FlashcardSet *flashcard_set, int order[], bool shuffle, bool star_only);

void flashcard(FlashcardSet *flashcard_set);

void type(FlashcardSet *flashcard_set);
void multipleChoice(FlashcardSet *flashcard_set);
