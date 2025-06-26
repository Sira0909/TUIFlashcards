#include "vocabList.h"
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

VocabList* create_Vocab_List_Object(){
    VocabList* vocablist = (VocabList*)malloc(sizeof(VocabList));

    vocablist->numItems = 0;

    vocablist->capacity = 32;
    
    vocablist->pairs = (VocabPair* )calloc(vocablist->capacity, sizeof(VocabPair));

    return vocablist;
}

int addpair(VocabList* vocablist, char name[MAX_VOCAB_LIST_ITEM_SIZE], char definition[MAX_VOCAB_LIST_DEFN_SIZE], int starred){
    if (vocablist->numItems >= vocablist->capacity){
        VocabPair* newPairs = (VocabPair* )realloc(vocablist->pairs,sizeof(VocabPair) * vocablist->capacity * 2);
        if (newPairs == NULL){
            return -1;
        }
        vocablist->capacity *= 2;
        vocablist->pairs = newPairs;
    }
    strcpy(vocablist->pairs[vocablist->numItems].name,  name);
    strcpy(vocablist->pairs[vocablist->numItems].definition,  definition);
    vocablist->pairs[vocablist->numItems].is_starred = starred;
    vocablist->numItems++;
    return 0;
}
void deletepair(VocabList* vocablist, int index){
    if (index < vocablist->numItems){
        for(int i = index; i<vocablist->numItems-1; i++){
            vocablist->pairs[i] = vocablist->pairs[i+1] ;
        }
    }
    vocablist->numItems--;
    strcpy(vocablist->pairs[vocablist->numItems].name, "");
    strcpy(vocablist->pairs[vocablist->numItems].definition, "");
    
    vocablist->pairs[vocablist->numItems].is_starred = 0;

}

void deleteListPointer(VocabList **vocablist){
    free((*vocablist)->pairs);
    (*vocablist)->pairs = NULL;
    free(*vocablist);
    *vocablist = NULL;
}
int writeVocabList(VocabList* vocablist, char filePath[VOCABFILESIZE], int todelete){
    FILE* VocabFile = fopen(filePath, "w");

    if(VocabFile == NULL){
        return -1;
    }
    fprintf(VocabFile, "%d", vocablist->numItems);
    for(int i = 0; i<vocablist->numItems;i++){
        fprintf(VocabFile, "\n%s\n%s\n%d", vocablist->pairs[i].name, vocablist->pairs[i].definition, vocablist->pairs[i].is_starred);
    }
    fclose(VocabFile);

    if (todelete == 1){
        deleteListPointer(&vocablist); }
    return 1;
}

int fillVocabList(VocabList* vocablist, char filePath[VOCABFILESIZE]){
    FILE *VocabFile = fopen(filePath, "r");
    
    if(VocabFile == NULL){
        return -1;
    }

    int numItems = 0; 
    fscanf(VocabFile, "%d\n", &numItems);

    for(int i = 0; i < numItems;i ++){
        char name[MAX_VOCAB_LIST_ITEM_SIZE];
        char defn[MAX_VOCAB_LIST_DEFN_SIZE];
        int starred = 0;
        if(fgets(name, MAX_VOCAB_LIST_ITEM_SIZE, VocabFile) == NULL) break;
        if(fgets(defn, MAX_VOCAB_LIST_DEFN_SIZE, VocabFile) == NULL) break;
        trim_whitespaces(name);
        trim_whitespaces(defn);
        if (fscanf(VocabFile, "%d\n", &starred) == EOF){
            break;
        }
        bool starbool = (starred == 1) ? true : false;
        if(addpair(vocablist, name, defn, starbool) == -1){
            return -1;
        }
    }
    fclose(VocabFile);
    return 1;
}



void getpairslimiter(VocabList* vocablist, char (*isstarred), char items[][128],char definitions[][128]){
    for(int i = 0; i< vocablist->numItems; i++){
        if(vocablist->pairs[i].is_starred){
            isstarred[i] = '*';
        }
        else isstarred[i] = ' ';
        strncpy(items[i],vocablist->pairs[i].name, 127);
        strncpy(definitions[i],vocablist->pairs[i].definition,127);
    }
}

