#ifndef HELPERSH
#define HELPERSH 1
// macro signum function
#define sign(x) \
    ({ __typeof__ (x) _x = (x); \
        _x>0 ? 1 : _x<0 ? -1 : 0; })
// macro max function
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
//for keybinds
#define ctrl(x)           ((x) & 0x1f)

#include <string.h>
void list_keybinds(int numBinds, char (*keybinds)[2][20]);
static char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(!strcmp(str," "))
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

#endif
