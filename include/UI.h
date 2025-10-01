
// main.c
char* getString(char* title, int maxsize);

// UI/lists.c
char* getLists(void (*to_call)(char*)) ;


// UI/editor.c
void editList(char ListName[]);

void addList();


void list_keybinds(int numBinds, char (*keybinds)[2][20]);
