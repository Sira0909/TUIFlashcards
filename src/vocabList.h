#ifndef VOCABLISTH
#define VOCABLISTH
#include "constants.h"



typedef struct _CONFIG{ //structure to hold config variables
    char configDIR[128];
    char vocabDIR[128];
} CONFIGSTRUCT;

typedef struct _VOCABPAIR{
    char name[MAX_VOCAB_LIST_ITEM_SIZE];
    char definition[MAX_VOCAB_LIST_DEFN_SIZE];
    bool is_starred;
} VocabPair;
typedef struct _VOCABLIST{
    int numItems;
    int capacity;
    VocabPair* pairs;
} VocabList;
    
VocabList* create_Vocab_List_Object();

int addpair(VocabList* vocablist, char name[MAX_VOCAB_LIST_ITEM_SIZE], char definition[MAX_VOCAB_LIST_DEFN_SIZE], int starred);

void deletepair(VocabList* vocablist, int index);

void deleteListPointer(VocabList **vocablist);

int writeVocabList(VocabList* vocablist, char filePath[VOCABFILESIZE], int todelete);

int fillVocabList(VocabList* vocablist, char filePath[VOCABFILESIZE]);

void getpairs(VocabList* vocablist, char items[][MAX_VOCAB_LIST_ITEM_SIZE+1],char definitions[][MAX_VOCAB_LIST_DEFN_SIZE+1]);

void getpairslimiter(VocabList* vocablist, char (*isstarred), char items[][128],char definitions[][128]);

#endif
