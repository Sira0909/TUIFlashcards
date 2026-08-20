#include <stdlib.h>
#include <string.h>

char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(!strcmp(str," ") || !strcmp(str, "\n"))
		str++;

	if(*str == '\0') // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str 
        &&   (!strcmp(end," ")      // trim space
        ||    !strcmp(end,"\n")))   // trim newline
                    end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

int is_all_space(char *string){
    for (int i = 0; string[i] != '\0'; i++){
        if(string[i]!= ' ') return 0;
    }
    return 1;
}





void shufflePreserveGraphemes(char* string){
    wchar_t splitgraphemes[strlen(string)];

    int j=0;
    for(int i = 0 ; i <strlen(string);i++){
        if(string[i] == 195-256){
            splitgraphemes[j] = 0xC300+string[i+1];
            i++;
        }
        else
            splitgraphemes[j] = string[i];
        j++;
    }
    for(int i = 0; i < j; i++){
        int ind = rand()%j;
        wchar_t buf = splitgraphemes[ind];
        if(splitgraphemes[ind]!=' ' && splitgraphemes[i]!=' '){
        splitgraphemes[ind]=splitgraphemes[i];
        splitgraphemes[i]=buf;
        }
    }
    for(int i = strlen(string)-1 ; i >=0;i--){
        j--;
        if(splitgraphemes[j]>0xFF){
            string[i]=splitgraphemes[j]%0x100;
            string[i-1] = -61;
            i--;
        }
        else
            string[i] = splitgraphemes[j];
    }
}
