#ifndef UIH
#define UIH
struct hook {
    int trigger;
    int (*effect)(void*);
};
// main.c
char* getString(char* title, int maxsize, char* defaultString);

// UI/lists.c
char* getLists(void (*to_call)(char*)) ;


// UI/editor.c
void editList(char ListName[]);

void addList(char* dir);
void addDir(char* dir);
int getConfirmation(char *question, char* successmsg, char* failmsg);


//keybinds is a variable-length array of pairs of pointers
//its basically a char (*)[7][20] but the last one has to be a pointer so that str_{u,d,l,r}key work
void list_keybinds(int numBinds, char *((*keybinds)[2]));
void showmsg(char* msg);
#endif
