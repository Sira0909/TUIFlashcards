#ifndef UIH
#define UIH
struct hook {
    char trigger;
    int (*effect)(void*);
};
// main.c
char* getString(char* title, int maxsize, char* defaultString);

// UI/lists.c
char* getLists(void (*to_call)(char*)) ;


// UI/editor.c
void editList(char ListName[]);

void addList();


void list_keybinds(int numBinds, char (*keybinds)[2][20]);
#endif
