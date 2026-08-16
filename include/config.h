#ifndef _CONFIGH
#define _CONFIGH

#include <limits.h>
#include <stdbool.h>
#ifndef PATH_MAX
#define PATH_MAX 164
#endif

typedef enum keyset{
    KEYBOARD_UNSET, //default is arrow
    KEYBOARD_ARROW,
    KEYBOARD_NVIM ,
    KEYBOARD_IJKL ,
} Layout;
typedef struct _keyboard{
    Layout layout;
    int dkey;
    int ukey;
    int lkey;
    int rkey;
    char str_dkey[20];
    char str_ukey[20];
    char str_lkey[20];
    char str_rkey[20];
} KEYBOARD;
                        
                        
                        

typedef struct _CONFIG{ //structure to hold config variables
    KEYBOARD keylayout;
    char config_dir[PATH_MAX];
    char flashcard_dir[PATH_MAX];
    int showKeybindsHelp;
    int autoaccent;
} CONFIGSTRUCT;

extern CONFIGSTRUCT config; // structure instance
                           


int get_config_struct(CONFIGSTRUCT *config);

int writeconfig();
void setkeylayout(CONFIGSTRUCT *config, Layout new_layout);

bool get_global_settings();
#endif
