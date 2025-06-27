#include "flashcards.h"
#include "helpers.h"
#include <stdio.h>

static char* trim_whitespaces(char *str) // trims whitespace from a string
{
	char *end;

	// trim leading space
	while(!strcmp(str," "))
		str++;

	if(*str == '\0') // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && (!strcmp(end," ") || !strcmp(end,"\n"))) end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

FlashcardSet* create_Flashcard_Set_Object(){
    FlashcardSet* flashcardset = (FlashcardSet*)malloc(sizeof(FlashcardSet));

    flashcardset->numItems = 0;

    flashcardset->capacity = 32;
    
    flashcardset->cards = (Flashcard* )calloc(flashcardset->capacity, sizeof(Flashcard));

    return flashcardset;
}

int addcard(FlashcardSet* flashcardset, char name[MAX_FLASHCARD_SET_ITEM_SIZE], char definition[MAX_FLASHCARD_SET_DEFN_SIZE], int starred){
    if (flashcardset->numItems >= flashcardset->capacity){
        Flashcard* newPairs = (Flashcard* )realloc(flashcardset->cards,sizeof(Flashcard) * flashcardset->capacity * 2);
        if (newPairs == NULL){
            return -1;
        }
        flashcardset->capacity *= 2;
        flashcardset->cards = newPairs;
    }
    strcpy(flashcardset->cards[flashcardset->numItems].name,  name);
    strcpy(flashcardset->cards[flashcardset->numItems].definition,  definition);
    flashcardset->cards[flashcardset->numItems].is_starred = starred;
    flashcardset->numItems++;
    return 0;
}
void deletecard(FlashcardSet* flashcardset, int index){
    if (index < flashcardset->numItems){
        for(int i = index; i<flashcardset->numItems-1; i++){
            flashcardset->cards[i] = flashcardset->cards[i+1] ;
        }
    }
    flashcardset->numItems--;
    if(flashcardset->numItems<0) flashcardset->numItems = 0;
    strcpy(flashcardset->cards[flashcardset->numItems].name, "");
    strcpy(flashcardset->cards[flashcardset->numItems].definition, "");
    
    flashcardset->cards[flashcardset->numItems].is_starred = 0;

}

void deleteSetPointer(FlashcardSet **flashcardset){
    free((*flashcardset)->cards);
    (*flashcardset)->cards = NULL;
    free(*flashcardset);
    *flashcardset = NULL;
}
int writeFlashcardSet(FlashcardSet* flashcardset, char filePath[FLASHCARDFILESIZE], int todelete){
    FILE* VocabFile = fopen(filePath, "w");

    if(VocabFile == NULL){
        return -1;
    }
    fprintf(VocabFile, "%d", flashcardset->numItems);
    for(int i = 0; i<flashcardset->numItems;i++){
        fprintf(VocabFile, "\n%s\n%s\n%d", flashcardset->cards[i].name, flashcardset->cards[i].definition, flashcardset->cards[i].is_starred);
    }
    fclose(VocabFile);

    if (todelete == 1){
        deleteSetPointer(&flashcardset); }
    return 1;
}

int fillFlashcardSet(FlashcardSet* flashcardset, char filePath[FLASHCARDFILESIZE]){
    FILE *VocabFile = fopen(filePath, "r");
    
    if(VocabFile == NULL){
        return -1;
    }

    int numItems = 0; 
    fscanf(VocabFile, "%d\n", &numItems);

    for(int i = 0; i < numItems;i ++){
        char name[MAX_FLASHCARD_SET_ITEM_SIZE];
        char defn[MAX_FLASHCARD_SET_DEFN_SIZE];
        int starred = 0;
        if(fgets(name, MAX_FLASHCARD_SET_ITEM_SIZE, VocabFile) == NULL) break;
        if(fgets(defn, MAX_FLASHCARD_SET_DEFN_SIZE, VocabFile) == NULL) break;
        trim_whitespaces(name);
        trim_whitespaces(defn);
        if (fscanf(VocabFile, "%d\n", &starred) == EOF){
            break;
        }
        bool starbool = (starred == 1) ? true : false;
        if(addcard(flashcardset, name, defn, starbool) == -1){
            return -1;
        }
    }
    fclose(VocabFile);
    return 1;
}



void getpairslimiter(FlashcardSet* flashcardset, char (*isstarred), char items[][128],char definitions[][128]){
    for(int i = 0; i< flashcardset->numItems; i++){
        if(flashcardset->cards[i].is_starred){
            isstarred[i] = '*';
        }
        else isstarred[i] = ' ';
        strncpy(items[i],flashcardset->cards[i].name, 127);
        strncpy(definitions[i],flashcardset->cards[i].definition,127);
    }
}

