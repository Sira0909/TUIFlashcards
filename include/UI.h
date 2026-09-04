#ifndef UIH
#define UIH
//keybind array structure:
//
// _keybind[0]: {array_size, NULL, NULL, render_funct()}
//
// _keybind[1..array_size]: {key, name, desc, function}
struct _keybind {
	int keycode;
	char* name;
	char* desc;
        int (*effect)(void* structure);
};

#define bind_keys(name, render_funct, count)\
    struct _keybind name[count] = {\
        {count, NULL, NULL, &render_funct},

// main.c
char* getString(char* title, int maxsize, char* defaultString);

// UI/lists.c
char* getLists(void (*to_call)(char*)) ;


// UI/editor.c
void editList(char ListName[]);

void addList(char* dir);
void addDir(char* dir);
int getConfirmation(char *question, char* successmsg, char* failmsg);


void list_keybinds(struct _keybind* binds);
void showmsg(char* msg);

int quit(void* window);
int run(void* window, struct _keybind* binds);
#endif
