#ifndef _CONFIGH
#define _CONFIGH

#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 164
#endif


typedef struct _CONFIG{ //structure to hold config variables
    char config_dir[PATH_MAX];
    char flashcard_dir[PATH_MAX];
    int showKeybindsHelp;
    int autoaccent;
} CONFIGSTRUCT;

extern CONFIGSTRUCT config; // structure instance
                           


int get_config_struct(CONFIGSTRUCT *config);


#endif
