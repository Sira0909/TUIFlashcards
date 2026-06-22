#ifndef FLASHCARDSH
#define FLASHCARDSH
#define MAX_FLASHCARD_SET_ITEM_SIZE 64
#define MAX_FLASHCARD_SET_DEFN_SIZE 128
#include <macros.h>
#include <stdbool.h>
#include <const.h>



typedef struct _FLASHCARDPAIR{  // flashcard, front and back
    bool is_starred;
    int num_columns;
    char term[MAX_FLASHCARD_SET_ITEM_SIZE];
    char (*definition)[128];
} Flashcard;
typedef struct _FLASHCARDSET{   // vector of flashcards
    int num_items;
    int num_columns;
    int capacity;
    Flashcard* cards;
} FlashcardSet;
    
FlashcardSet* create_Flashcard_Set_Object();

int addcard(FlashcardSet* flashcardset, char name[MAX_FLASHCARD_SET_ITEM_SIZE], char definition[][MAX_FLASHCARD_SET_DEFN_SIZE], int starred);
int addDefn(FlashcardSet* flashcard_set, char definitions[][MAX_FLASHCARD_SET_DEFN_SIZE]);
int delDefn(FlashcardSet* flashcard_set, int index);

void deletecard(FlashcardSet* flashcardset, int index);

void deleteSetPointer(FlashcardSet **flashcardset);

int writeFlashcardSet(FlashcardSet* flashcardset, char filePath[PATH_MAX], int todelete);

int fillFlashcardSet(FlashcardSet* flashcardset, char filePath[PATH_MAX]);

void getpairs(FlashcardSet* flashcardset, char items[][MAX_FLASHCARD_SET_ITEM_SIZE+1],char definitions[][MAX_FLASHCARD_SET_DEFN_SIZE+1]);

void getpairslimiter(FlashcardSet* flashcardset, char isstarred[], char items[][128],char definitions[][128], int index);

void getDefinitionList(FlashcardSet* flashcard_set, int index,char (*(definitions))[128]);


#endif
