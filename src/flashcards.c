
#include <linux/limits.h>
#include <macros.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <flashcards.h>

//constructor
FlashcardSet* create_Flashcard_Set_Object(){
    FlashcardSet* flashcard_set = (FlashcardSet*)malloc(sizeof(FlashcardSet));

    flashcard_set->num_items = 0;

    //default 32 max items
    flashcard_set->capacity = 32;
    
    flashcard_set->num_columns = 2;
    flashcard_set->cards = (Flashcard* )calloc(flashcard_set->capacity, sizeof(Flashcard));

    return flashcard_set;
}

int addcard(FlashcardSet* flashcard_set, char term[MAX_FLASHCARD_SET_ITEM_SIZE], char definition[][MAX_FLASHCARD_SET_DEFN_SIZE], int starred){
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
    if(flashcard_set->num_columns<2){
        return -1;
    }
    // create and set new flashcard
    strcpy(flashcard_set->cards[flashcard_set->num_items].term,  term);
    flashcard_set->cards[flashcard_set->num_items].definition = (char(*)[MAX_FLASHCARD_SET_DEFN_SIZE]) calloc(flashcard_set->num_columns-1,sizeof(char[MAX_FLASHCARD_SET_DEFN_SIZE]));
    for(int i = 0; i<flashcard_set->num_columns-1;i++)
        strcpy(flashcard_set->cards[flashcard_set->num_items].definition[i],  definition[i]);
    flashcard_set->cards[flashcard_set->num_items].is_starred = starred;
    flashcard_set->num_items++;

    return 0;
}
int addDefn(FlashcardSet* flashcard_set, char definitions[][MAX_FLASHCARD_SET_DEFN_SIZE]){
    flashcard_set->num_columns++;
    for(int i =0; i < flashcard_set->num_items; i++){
        flashcard_set->cards[i].definition = (char(*)[MAX_FLASHCARD_SET_DEFN_SIZE]) realloc(flashcard_set->cards[i].definition,(flashcard_set->num_columns-1)*sizeof(char[MAX_FLASHCARD_SET_DEFN_SIZE]));
        if (flashcard_set->cards->definition==NULL){
            return -1;
        }
        if(definitions ==NULL){
            strcpy(flashcard_set->cards[i].definition[flashcard_set->num_columns-2], flashcard_set->cards[i].definition[0]); //copy primary definition
        }
        else{
            strcpy(flashcard_set->cards[i].definition[flashcard_set->num_columns-2], definitions[i]); //use definitions list
        }
    }
    return 1;
}
int delDefn(FlashcardSet* flashcard_set, int index){
    if(flashcard_set->num_columns>2){
        flashcard_set->num_columns--;
        for(int i =0; i < flashcard_set->num_items; i++){
            for(int j = index; j<flashcard_set->num_columns-2; j++){
                strcpy(flashcard_set->cards[i].definition[j], flashcard_set->cards[i].definition[j+1]);
            }
            flashcard_set->cards[i].definition = (char(*)[MAX_FLASHCARD_SET_DEFN_SIZE]) realloc(flashcard_set->cards[i].definition,(flashcard_set->num_columns-1)*sizeof(char[MAX_FLASHCARD_SET_DEFN_SIZE]));
            if (flashcard_set->cards->definition==NULL){
                return -1;
            }
        }
    }
    return 1;
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
    strcpy(flashcard_set->cards[flashcard_set->num_items].term, "");
    for(int i = 0; i<flashcard_set->num_columns-1;i++)
        strcpy(flashcard_set->cards[flashcard_set->num_items].definition[i], "");
    free(flashcard_set->cards[flashcard_set->num_items].definition);
    
    flashcard_set->cards[flashcard_set->num_items].is_starred = 0;

}

// frees flashcard set
void deleteSetPointer(FlashcardSet **flashcard_set){
    for(int i = 0; i<(*flashcard_set)->num_items;i++)
        free((*flashcard_set)->cards[i].definition);
    free((*flashcard_set)->cards);
    (*flashcard_set)->cards = NULL;
    free(*flashcard_set);
    *flashcard_set = NULL;
}




/*
 * Flashcard format:
 *
 * char* term
 * char* defn1
 * ...
 * int starred
 */
/* Flashcard list format:
 * 
 * "Version":<int>
 * "Columns":<int> (default 2)
 * "Terms":<int>
 * "Flashcards": array of flashcards
 */
// writes flashcard set to file 
int writeFlashcardSet(FlashcardSet* flashcard_set, char filePath[PATH_MAX], int todelete){
    if(strcmp(filePath+strlen(filePath)-5, ".list")){
        strcat(filePath, ".list");
        
    }
    FILE* VocabFile = fopen(filePath, "w");

    if(VocabFile == NULL){
        return -1;
    }
    fprintf(VocabFile, "\"Version:\"%d\n", 1); //denotes flashcard version
    fprintf(VocabFile, "\"Columns:\"%d\n", flashcard_set->num_columns);
    fprintf(VocabFile, "\"Terms\":%d\n", flashcard_set->num_items);
    fprintf(VocabFile, "\"Flashcards\":{");
    for(int i = 0; i<flashcard_set->num_items;i++){
        fprintf(VocabFile, "\n%s", flashcard_set->cards[i].term);
        for(int j = 0; j<flashcard_set->num_columns-1;j++)
            fprintf(VocabFile, "\n%s", flashcard_set->cards[i].definition[j]);
        fprintf(VocabFile, "\n%d",  flashcard_set->cards[i].is_starred);
    }
    fprintf(VocabFile, "\n}");
    fclose(VocabFile);

    if (todelete == 1){
        deleteSetPointer(&flashcard_set); }
    return 1;
}

int updateList(char* filePath){
    //read old format
    FILE *VocabFile = fopen(filePath, "r");
    
    if(VocabFile == NULL){
        return -1;
    }
    FlashcardSet* flashcard_set = create_Flashcard_Set_Object();
    flashcard_set->num_columns = 2;

    int num_items = 0; 
    fscanf(VocabFile, "%d\n", &num_items);

    for(int i = 0; i < num_items;i ++){
        char term[MAX_FLASHCARD_SET_ITEM_SIZE];
        char defn[1][MAX_FLASHCARD_SET_DEFN_SIZE];
        int starred = 0;

        //end of file?
        if(fgets(term, MAX_FLASHCARD_SET_ITEM_SIZE, VocabFile) == NULL) break; 
        if(fgets(defn[0], MAX_FLASHCARD_SET_DEFN_SIZE, VocabFile) == NULL) break;

        // make sure to remove excess whitespace
        trim_whitespaces(term);
        trim_whitespaces(defn[0]);
        // actual eof check
        if (fscanf(VocabFile, "%d\n", &starred) == EOF){
            break;
        }
        bool starbool = (starred == 1) ? true : false;
        if(addcard(flashcard_set, term, defn, starbool) == -1){
            return -1;
        }
    }
    fclose(VocabFile);
    char newfilePath[PATH_MAX];
    strncpy(newfilePath, filePath, PATH_MAX);
    strcat(newfilePath, ".list");
    writeFlashcardSet(flashcard_set, newfilePath, 1);
    return 1;
}



// gets items and definitions from a flashcardset
void getpairslimiter(FlashcardSet* flashcard_set, char isstarred[], char items[][128],char definitions[][128], int index){
    for(int i = 0; i< flashcard_set->num_items; i++){
        if(flashcard_set->cards[i].is_starred){
            isstarred[i] = '*';
        }
        else isstarred[i] = ' ';
        strncpy(items[i],flashcard_set->cards[i].term, 127);
        strncpy(definitions[i],flashcard_set->cards[i].definition[index],127);
    }
}

void getDefinitionList(FlashcardSet* flashcard_set, int index,char (*(definitions))[128]){
    for(int i = 0; i< flashcard_set->num_items; i++){
        strncpy(definitions[i],flashcard_set->cards[i].definition[index],127);
    }
}
// gets flashcard set from file
int fillFlashcardSet(FlashcardSet* flashcard_set, char filePath[PATH_MAX]){
    if(strcmp(filePath+strlen(filePath)-5, ".list")){
        if(updateList(filePath)==1){
            remove(filePath);
        }
        else{
            return -1;
        }
        strcat(filePath, ".list");
        
    }
    FILE *VocabFile = fopen(filePath, "r");
    
    if(VocabFile == NULL){
        return -1;
    }

    int version;
    int num_items = 0; 
    fscanf(VocabFile, "\"Version:\"%d\n", &version); //denotes flashcard version
    fscanf(VocabFile, "\"Columns:\"%d\n", &(flashcard_set->num_columns));
    fscanf(VocabFile, "\"Terms\":%d\n", &num_items);


    fscanf(VocabFile, "\"Flashcards\":{\n");



    for(int i = 0; i < num_items;i ++){
        char term[MAX_FLASHCARD_SET_ITEM_SIZE];
        char defn[flashcard_set->num_columns-1][MAX_FLASHCARD_SET_DEFN_SIZE];
        int starred = 0;

        //end of file?
        if(fgets(term, MAX_FLASHCARD_SET_ITEM_SIZE, VocabFile) == NULL) break; 
        trim_whitespaces(term);
        for(int j = 0; j<flashcard_set->num_columns-1;j++){
            if(fgets(defn[j], MAX_FLASHCARD_SET_DEFN_SIZE, VocabFile) == NULL) break;
            trim_whitespaces(defn[j]);
        }

        // make sure to remove excess whitespace
        // actual eof check
        if (fscanf(VocabFile, "%d\n", &starred) == EOF){
            break;
        }
        bool starbool = (starred == 1) ? true : false;
        if(addcard(flashcard_set, term, defn, starbool) == -1){
            return -1;
        }
    }
    fclose(VocabFile);
    return 1;
}



