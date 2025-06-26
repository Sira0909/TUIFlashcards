#ifndef FLASHCARDSH
#define FLASHCARDSH
#include "constants.h"



typedef struct _CONFIG{ //structure to hold config variables
    char configDIR[128];
    char flashcardDIR[128];
} CONFIGSTRUCT;

typedef struct _FLASHCARDPAIR{
    char name[MAX_FLASHCARD_SET_ITEM_SIZE];
    char definition[MAX_FLASHCARD_SET_DEFN_SIZE];
    bool is_starred;
} Flashcard;
typedef struct _FLASHCARDSET{
    int numItems;
    int capacity;
    Flashcard* cards;
} FlashcardSet;
    
FlashcardSet* create_Flashcard_Set_Object();

int addcard(FlashcardSet* flashcardset, char name[MAX_FLASHCARD_SET_ITEM_SIZE], char definition[MAX_FLASHCARD_SET_DEFN_SIZE], int starred);

void deletecard(FlashcardSet* flashcardset, int index);

void deleteSetPointer(FlashcardSet **flashcardset);

int writeFlashcardSet(FlashcardSet* flashcardset, char filePath[FLASHCARDFILESIZE], int todelete);

int fillFlashcardSet(FlashcardSet* flashcardset, char filePath[FLASHCARDFILESIZE]);

void getpairs(FlashcardSet* flashcardset, char items[][MAX_FLASHCARD_SET_ITEM_SIZE+1],char definitions[][MAX_FLASHCARD_SET_DEFN_SIZE+1]);

void getpairslimiter(FlashcardSet* flashcardset, char (*isstarred), char items[][128],char definitions[][128]);

#endif
