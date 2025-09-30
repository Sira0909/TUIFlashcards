#include <macros.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <config.h>

void get_config_struct(CONFIGSTRUCT *config){
    //get directory for config files
    char *config_DIR = (char *) calloc(128, sizeof(char));

    //start by checking $XDG_CONFIG_HOME environment var
    char *conf_home = getenv("XDG_CONFIG_HOME");

    //if $XDG_CONFIG_HOME has been set, set config_DIR to it.
    if(conf_home != NULL){
        strncpy(config_DIR,conf_home, 113);
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
        strncpy(config_DIR,conf_home, 105);
        strcat(config_DIR, "/.config");
    }
    strcat(config_DIR, "/TUIFlashcards");

    //file name for config file
    char cFile[135];

    //start with config_DIR, add /config to end. should now be $XDG_CONFIG_DIR/TUIFlashcards/config
    strcpy(cFile, config_DIR); strcat(cFile,"/config");
    FILE *config_File;

    //check if file already exists. if not, create it
    if(!(config_File = fopen(cFile, "r"))){
        mkdir(config_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); // "mkdir configDIR"
        FILE* writeconfig = fopen(cFile, "w"); // make config file

        fprintf(writeconfig, "%s/Lists/", config_DIR);

        fclose(writeconfig);
        
        
        // if file still doesnt exist, error
        if(!(config_File = fopen(cFile, "r"))){printf("error occured making config file"); free(config_DIR); exit(-1);} 

    }
    // set config struct's config_DIR to configDIR
    strcpy(config->config_dir,config_DIR);

    // get flashcardDIR from config file
    fgets(config->flashcard_dir, 128, config_File);
    strcpy(config->flashcard_dir , trim_whitespaces(config->flashcard_dir));
    free(config_DIR);
}
