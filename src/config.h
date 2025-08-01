#ifndef _CONFIGH
#define _CONFIGH
typedef struct _CONFIG{ //structure to hold config variables
    char config_dir[128];
    char flashcard_dir[128];
} CONFIGSTRUCT;

void get_config_struct(CONFIGSTRUCT *config);

static CONFIGSTRUCT config; // structure instance
#endif
