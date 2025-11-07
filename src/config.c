#include <limits.h>
#include <linux/limits.h>
#include <macros.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <config.h>

FILE* get_config_file(CONFIGSTRUCT *config){
    //get directory for config files
    char *config_DIR = (char *) calloc(PATH_MAX, sizeof(char));

    //start by checking $XDG_CONFIG_HOME environment var
    char *conf_home = getenv("XDG_CONFIG_HOME");

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
            if (mkdir(config_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1){ // "mkdir configDIR"
                printf("error occurred while creating config directory. does ~/.config exist?");
                free(config_DIR); 
                exit(-1);
            }
            FILE* writeconfig = fopen(cFile, "w"); // make config file
        }

        fprintf(writeconfig, "// where to store flashcards\n");

        fprintf(writeconfig, "flashcard_dir: %s/Lists/\n\n", config_DIR);
        fprintf(writeconfig, "// whether to show the dialog explaining how to access keybinds\n"\
                             "show_keybinds_top: 1 \n"\
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
void fill_defaults(CONFIGSTRUCT *config){
    char cardDir[PATH_MAX];
    strcpy(cardDir,config->config_dir);
    strcat(cardDir, "/Lists");
    strcpy(config->flashcard_dir, cardDir);
    config->showKeybindsTop = 1;
    config->autoaccent = 1;
}
#define LINE_MAX_SIZE 128
int get_config_struct(CONFIGSTRUCT *config){
    FILE *config_File = get_config_file(config);

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
                else if(strcmp(trimmedsetting, "show_keybinds_top")==0){
                    if(line[i+2]=='1')
                        config->showKeybindsTop=1;
                    else if(line[i+2]=='0')
                        config->showKeybindsTop=0;
                    else {
                        printf("%s\n", line);
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
                            printf("%s\n", line);
                            countUndefconfigs++;
                            break;
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
