#ifndef UIH
#define UIH
//keybind array structure:
//
//
// _keybind[0]: {array_size, NULL, NULL, render_funct()}
//	    // the first keybind provides information, namely the number of keybinds and the function for rendering the window. bind_keys sets this up for you
//
// _keybind[1..array_size]: {key, name, desc, function}
//	    // key: the keycode that activates
//	    // name: the string representation of the key for list_keybinds to display
//	    // desc: the description for the bind for list_keybinds to display
//	    // function: the function that should be run. return code indicates whether to end the loop.
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
int makeSelection(int numOptions, char (*options)[128], char* title);

int quit(void* window);
int run(void* window, struct _keybind* binds);
#endif
