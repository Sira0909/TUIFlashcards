
#ifndef _KEYBINDSH
#define _KEYBINDSH
// lists of keybinds
static char mainkeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};
static char editkeybinds[13][2][20]= { 
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "edit text"},
    {" ", " "},
    {"s", "star flashcard"},
    {"a", "add flashcard"},
    {"d", "delete flashcard"},
    {" ", " "},
    {"w", "save"},
    {"q", "quit without save"},
    {"?", "list keybinds"}
};
static char selectionkeybinds[10][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select list"},
    {" ", " "},
    {"a", "add list"},
    {"d", "delete list"},
    {" ", " "},
    {"?", "list keybinds"}
};
static char settingskeybinds[10][2][20] = {
    {"j","down"},
    {"k","up"},
    {" ", " "},
    {"<enter>", "toggle"},
    {" ", " "},
    {"?", "list keybinds"}
};

static char playkeybinds[7][2][20] = {
    {"h", "left"},
    {"j","down"},
    {"k","up"},
    {"l","right"},
    {"<enter>", "select"},
    {" ", " "},
    {"?", "list keybinds"}
};

void list_keybinds(int numBinds, char (*keybinds)[2][20]);
#endif
