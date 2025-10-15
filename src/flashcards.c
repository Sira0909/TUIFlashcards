
#include <macros.h>
#include <stdlib.h>
#include <stdio.h>

#include <flashcards.h>

//constructor
FlashcardSet* create_Flashcard_Set_Object(){
    FlashcardSet* flashcard_set = (FlashcardSet*)malloc(sizeof(FlashcardSet));

    flashcard_set->num_items = 0;

    //default 32 max items
    flashcard_set->capacity = 32;
    
    flashcard_set->cards = (Flashcard* )calloc(flashcard_set->capacity, sizeof(Flashcard));

    return flashcard_set;
}

int addcard(FlashcardSet* flashcard_set, char name[MAX_FLASHCARD_SET_ITEM_SIZE], char definition[MAX_FLASHCARD_SET_DEFN_SIZE], int starred){
    //resize if necessary
    if (flashcard_set->num_items >= flashcard_set->capacity){

        //size doubles
        Flashcard* newPairs = (Flashcard* )realloc(flashcard_set->cards,sizeof(Flashcard) * flashcard_set->capacity * 2);
        //check realloc success
        if (newPairs == NULL){
            return -1;
            
        }

        //update fields
        flashcard_set->capacity *= 2;
        flashcard_set->cards = newPairs;
    }
    // create and set new flashcard
    strcpy(flashcard_set->cards[flashcard_set->num_items].name,  name);
    strcpy(flashcard_set->cards[flashcard_set->num_items].definition,  definition);
    flashcard_set->cards[flashcard_set->num_items].is_starred = starred;
    flashcard_set->num_items++;

    return 0;
}

// delete a flashcard
void deletecard(FlashcardSet* flashcard_set, int index){
    // move items after back
    if (index < flashcard_set->num_items){
        for(int i = index; i<flashcard_set->num_items-1; i++){
            flashcard_set->cards[i] = flashcard_set->cards[i+1] ;
        }
    }
    // update fields and last card
    flashcard_set->num_items--;
    if(flashcard_set->num_items<0) flashcard_set->num_items = 0;
    strcpy(flashcard_set->cards[flashcard_set->num_items].name, "");
    strcpy(flashcard_set->cards[flashcard_set->num_items].definition, "");
    
    flashcard_set->cards[flashcard_set->num_items].is_starred = 0;

}

// frees flashcard set
void deleteSetPointer(FlashcardSet **flashcard_set){
    free((*flashcard_set)->cards);
    (*flashcard_set)->cards = NULL;
    free(*flashcard_set);
    *flashcard_set = NULL;
}

// writes flashcard set to file 
int writeFlashcardSet(FlashcardSet* flashcard_set, char filePath[PATH_MAX], int todelete){
    FILE* VocabFile = fopen(filePath, "w");

    if(VocabFile == NULL){
        return -1;
    }
    fprintf(VocabFile, "%d", flashcard_set->num_items);
    for(int i = 0; i<flashcard_set->num_items;i++){
        fprintf(VocabFile, "\n%s\n%s\n%d", flashcard_set->cards[i].name, flashcard_set->cards[i].definition, flashcard_set->cards[i].is_starred);
    }
    fclose(VocabFile);

    if (todelete == 1){
        deleteSetPointer(&flashcard_set); }
    return 1;
}

// gets flashcard set from file
int fillFlashcardSet(FlashcardSet* flashcard_set, char filePath[PATH_MAX]){
    FILE *VocabFile = fopen(filePath, "r");
    
    if(VocabFile == NULL){
        return -1;
    }

    int num_items = 0; 
    fscanf(VocabFile, "%d\n", &num_items);

    for(int i = 0; i < num_items;i ++){
        char name[MAX_FLASHCARD_SET_ITEM_SIZE];
        char defn[MAX_FLASHCARD_SET_DEFN_SIZE];
        int starred = 0;

        //end of file?
        if(fgets(name, MAX_FLASHCARD_SET_ITEM_SIZE, VocabFile) == NULL) break; 
        if(fgets(defn, MAX_FLASHCARD_SET_DEFN_SIZE, VocabFile) == NULL) break;

        // make sure to remove excess whitespace
        trim_whitespaces(name);
        trim_whitespaces(defn);
        // actual eof check
        if (fscanf(VocabFile, "%d\n", &starred) == EOF){
            break;
        }
        bool starbool = (starred == 1) ? true : false;
        if(addcard(flashcard_set, name, defn, starbool) == -1){
            return -1;
        }
    }
    fclose(VocabFile);
    return 1;
}



// gets items and definitions from a flashcardset
void getpairslimiter(FlashcardSet* flashcard_set, char isstarred[], char items[][128],char definitions[][128]){
    for(int i = 0; i< flashcard_set->num_items; i++){
        if(flashcard_set->cards[i].is_starred){
            isstarred[i] = '*';
        }
        else isstarred[i] = ' ';
        strncpy(items[i],flashcard_set->cards[i].name, 127);
        strncpy(definitions[i],flashcard_set->cards[i].definition,127);
    }
}
