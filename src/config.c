#include <limits.h>

#ifdef _WIN32
#include <limits.h>
#endif
#ifndef _WIN32
#include <linux/limits.h>
#endif
#include <macros.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <config.h>
#include <curses.h>



FILE* get_config_file(CONFIGSTRUCT *config){
    //get directory for config files
    char *config_DIR = (char *) calloc(PATH_MAX, sizeof(char));
#ifndef _WIN32
    //start by checking $XDG_CONFIG_HOME environment var
    char *conf_home = getenv("XDG_CONFIG_HOME");
#endif
    //windows uses different env vars
#ifdef _WIN32
    char *conf_home = getenv("APPDATA");
#endif
    //if $XDG_CONFIG_HOME has been set, set config_DIR to it.
    if(conf_home != NULL){
        strncpy(config_DIR,conf_home, PATH_MAX);
    }
    else{
        //else, try $HOME env variable
        conf_home = getenv("HOME");
        if(conf_home == NULL){
            //if neither exist, error.
            free(config_DIR); 
            printf("Please set either a $HOME env variable or a $XDG_CONFIG_HOME env variable in order to make a configuration");
            exit(-1);
        }

        // if $HOME does exist, use it, add ".config", and set config_DIR to that.
        strncpy(config_DIR,conf_home, PATH_MAX);
        strcat(config_DIR, "/.config");
    }
    strcat(config_DIR, "/TUIFlashcards");

    //file name for config file
    char cFile[PATH_MAX];

    //start with config_DIR, add /config to end. should now be $XDG_CONFIG_DIR/TUIFlashcards/config
    strcpy(cFile, config_DIR); strcat(cFile,"/config");
    FILE *config_File;

    //check if file already exists. if not, create it
    if(!(config_File = fopen(cFile, "r"))){
        FILE* writeconfig;
        if(!(writeconfig = fopen(cFile, "w"))){
            if (makedir(config_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1){ // "mkdir configDIR"
                printf("error occurred while creating config directory. does ~/.config exist?");
                free(config_DIR); 
                exit(-1);
            }
            writeconfig = fopen(cFile, "w"); // make config file
        }

        fprintf(writeconfig, "// where to store flashcards\n");

        fprintf(writeconfig, "flashcard_dir: %s/Lists/\n\n", config_DIR);
        fprintf(writeconfig, "// whether to show the dialog explaining how to access keybinds\n"\
                             "show_keybinds_help: 1 \n"\
                             "\n"\
                             "// determines which keys are used for movement.\n"\
                             "// options are:\n"\
                             "//      \"arrow\" (arrow keys)\n"\
                             "//      \"ijkl\" (ijkl, similar to wasd but on the other side of the keyboard)\n"\
                             "//      \"nvim\" (hjkl)\n"\
                             "keyboard_layout: arrow\n"\
                             "\n"\
                             "// how to do accent marks:\n"\
                             "// 0: no accents\n"\
                             "// 1: `a->á, ~n->ñ, ``->`\n"\
                             "// maybe more to come\n"\
                             "auto_accent: 1");

        fclose(writeconfig);
        
        
        // if file still doesnt exist, error
        if(!(config_File = fopen(cFile, "r"))){printf("error occured making config file"); free(config_DIR); exit(-1);} 

    }
    strcpy(config->config_dir,config_DIR);
    free(config_DIR);
    return(config_File);

}

void setkeylayout(CONFIGSTRUCT *config, Layout new_layout){
    switch(new_layout){
        case KEYBOARD_ARROW:
            config->keylayout.layout=KEYBOARD_ARROW;
            config->keylayout.ukey=KEY_UP;
            config->keylayout.dkey=KEY_DOWN;
            config->keylayout.lkey=KEY_LEFT;
            config->keylayout.rkey=KEY_RIGHT;
            strcpy(config->keylayout.str_dkey, "down" );
            strcpy(config->keylayout.str_ukey, "up"   );
            strcpy(config->keylayout.str_lkey, "left" );
            strcpy(config->keylayout.str_rkey, "right");
            break;
        case KEYBOARD_IJKL:
            config->keylayout.layout=KEYBOARD_IJKL;
            config->keylayout.dkey  = 'k';
            config->keylayout.ukey    = 'i';
            config->keylayout.lkey  = 'j';
            config->keylayout.rkey = 'l';
            strcpy(config->keylayout.str_dkey ,"k");
            strcpy(config->keylayout.str_ukey   ,"i");
            strcpy(config->keylayout.str_lkey ,"j");
            strcpy(config->keylayout.str_rkey,"l");
            break;
        case KEYBOARD_NVIM:
            config->keylayout.layout=KEYBOARD_NVIM;
            config->keylayout.dkey = 'j';
            config->keylayout.ukey = 'k';
            config->keylayout.lkey = 'h';
            config->keylayout.rkey = 'l';
            strcpy(config->keylayout.str_dkey  ,"j");
            strcpy(config->keylayout.str_ukey    ,"k");
            strcpy(config->keylayout.str_lkey  ,"h");
            strcpy(config->keylayout.str_rkey ,"l");
            break;
        default:
            config->keylayout.layout=KEYBOARD_UNSET;
            config->keylayout.ukey=KEY_UP;
            config->keylayout.dkey=KEY_DOWN;
            config->keylayout.lkey=KEY_LEFT;
            config->keylayout.rkey=KEY_RIGHT;
            strcpy(config->keylayout.str_dkey, "down" );
            strcpy(config->keylayout.str_ukey, "up"   );
            strcpy(config->keylayout.str_lkey, "left" );
            strcpy(config->keylayout.str_rkey, "right");
            break;
    }
    

}

void fill_defaults(CONFIGSTRUCT *config){
    config->keylayout.layout=KEYBOARD_UNSET;
    config->keylayout.ukey=KEY_UP;
    config->keylayout.dkey=KEY_DOWN;
    config->keylayout.lkey=KEY_LEFT;
    config->keylayout.rkey=KEY_RIGHT;
    strcpy(config->keylayout.str_dkey, "down" );
    strcpy(config->keylayout.str_ukey, "up"   );
    strcpy(config->keylayout.str_lkey, "left" );
    strcpy(config->keylayout.str_rkey, "right");

    char cardDir[PATH_MAX];
    strcpy(cardDir,config->config_dir);
    strcat(cardDir, "/Lists");
    strcpy(config->flashcard_dir, cardDir);
    config->showKeybindsHelp = 1;
    config->autoaccent = 1;
}
#define LINE_MAX_SIZE 128
int get_config_struct(CONFIGSTRUCT *config){
    FILE *config_File = get_config_file(config);
    fill_defaults(config);

    char line[LINE_MAX_SIZE];
    int countUndefconfigs =0;
    while(1){
        if(fgets(line, LINE_MAX_SIZE, config_File)==NULL) break;
        //if(line[strlen(line)-1]=='\n') line[strlen(line)-1]='\0';
        if(*trim_whitespaces(line)=='\0')
            continue;
        if(line[0] == '/' && line[1] == '/'){
            continue; //comment
        }
        else{
            char setting[LINE_MAX_SIZE];
            char* trimmedsetting;
            strcpy(setting, line);
            int i =0;for(;i<LINE_MAX_SIZE&&setting[i]!=':'; i++);
            trimmedsetting=trim_whitespaces(setting);
            if(i==128){
                printf("%s\n", line);
                countUndefconfigs++;
            }
            else{
                setting[i]='\0';
                if(strcmp(trimmedsetting, "flashcard_dir")==0){
                    strcpy(config->flashcard_dir,line+i+2);
                }
                else if(strcmp(trimmedsetting, "show_keybinds_help")==0){
                    if(line[i+2]=='1')
                        config->showKeybindsHelp=1;
                    else if(line[i+2]=='0')
                        config->showKeybindsHelp=0;
                    else {
                        printf("undefined value: %s\n", line);
                        countUndefconfigs++;
                    }
                }
                else if (strcmp(trimmedsetting, "auto_accent")==0) {
                    switch (line[i+2]) {
                        case '0':
                            config->autoaccent=0;
                            break;
                        case '1':
                            config->autoaccent=1;
                            break;
                        case '2':
                            config->autoaccent=2;
                            break;
                        default:
                            printf("undefined value: %s\n", line);
                            countUndefconfigs++;
                            break;
                    }
                
                }
                else if (strcmp(trimmedsetting, "keyboard_layout")==0) {
                    if(strcmp(&(line[i+2]),"arrow")==0){
                        config->keylayout.layout=KEYBOARD_ARROW;
                        //rest are defaults
                    }
                    else if(strcmp(&(line[i+2]),"nvim")==0){
                        config->keylayout.layout=KEYBOARD_NVIM;
                        config->keylayout.dkey = 'j';
                        config->keylayout.ukey = 'k';
                        config->keylayout.lkey = 'h';
                        config->keylayout.rkey = 'l';
                        strcpy(config->keylayout.str_dkey  ,"j");
                        strcpy(config->keylayout.str_ukey    ,"k");
                        strcpy(config->keylayout.str_lkey  ,"h");
                        strcpy(config->keylayout.str_rkey ,"l");
                    }
                    else if(strcmp(&(line[i+2]),"ijkl")==0){
                        config->keylayout.layout=KEYBOARD_IJKL;
                        config->keylayout.dkey  = 'k';
                        config->keylayout.ukey    = 'i';
                        config->keylayout.lkey  = 'j';
                        config->keylayout.rkey = 'l';
                        strcpy(config->keylayout.str_dkey ,"k");
                        strcpy(config->keylayout.str_ukey   ,"i");
                        strcpy(config->keylayout.str_lkey ,"j");
                        strcpy(config->keylayout.str_rkey,"l");
                    }
                    else{
                        printf("undefined value: %s\n", &(line[i+2]));
                        countUndefconfigs++;
                    }
                }
                else {
                    printf("%s\n", line);
                    countUndefconfigs++;
                }

            }

        }
        

    }
    //strcpy(config->flashcard_dir , trim_whitespaces(config->flashcard_dir));
    return countUndefconfigs;
}

int writeconfig(){
    char confFile[PATH_MAX];
    strcpy(confFile, config.config_dir);
    strcat(confFile, "/config");

    FILE* conf = fopen(confFile, "w");

    if(conf == NULL){
        return -1;
    }

    char layout[6];
    switch (config.keylayout.layout){
        case KEYBOARD_UNSET:
        case KEYBOARD_ARROW:
            strcpy(layout, "arrow");
            break;
        case KEYBOARD_IJKL:
            strcpy(layout, "ijkl");
            break;
        case KEYBOARD_NVIM:
            strcpy(layout, "nvim");
            break;
        default:
            strcpy(layout, "arrow");
            break;
    }
    fprintf(conf, "\nflashcard_dir: %s", config.flashcard_dir);
    fprintf(conf, "\nkeyboard_layout: %s", layout);
    fprintf(conf, "\nshow_keybinds_help: %d", config.showKeybindsHelp);
    fprintf(conf, "\nauto_accent: %d", config.autoaccent);

    return 1;
}
