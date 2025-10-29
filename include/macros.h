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

char* trim_whitespaces(char *str);

int is_all_space(char *string);

#endif
