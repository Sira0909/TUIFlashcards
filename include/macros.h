#ifndef MACROS_H
#define MACROS_H 1

#define MAX_FLASHCARD_SET_ITEM_SIZE 64
#define MAX_FLASHCARD_SET_DEFN_SIZE 128

#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 164
#endif


// macro signum function
#define sign(x) \
    ({ __typeof__ (x) _x = (x); \
        _x>0 ? 1 : _x<0 ? -1 : 0; })
// macro max function
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })
//for keybinds
#define ctrl(x)           ((x) & 0x1f)

#include <string.h>

static char* trim_whitespaces(char *str)
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

static int is_all_space(char *string){
    for (int i = 0; string[i] != '\0'; i++){
        if(string[i]!= ' ') return 0;
    }
    return 1;
}
#endif
