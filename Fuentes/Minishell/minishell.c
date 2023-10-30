#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_LEN	1024
#define	LINE_LEN	1024
#define ARG_LEN	    16
#define ARGS	    5
#define SEPARATORS  " ,\t"

typedef struct{
  char *name;
  int argc;
  char *argv[ARGS];
}tCommand;

char *getPath(char cName[], char *path){
    char aux[PATH_LEN];
    strcpy(aux, path);
    strcat(aux, "/bin/");
    strcat(aux, cName);
    if(access(aux, F_OK) != -1){
        char * commandName = (char *) malloc(strlen(aux)+1);
        strcpy(commandName, aux);
        return commandName;		
    }else{
        return NULL;
    }
}

int getCommand(char *commandLine, tCommand *comand){
    int argc;
    argc = 0;
    char *token;
    token = strtok(commandLine, SEPARATORS);
    while( token != NULL ) {
        comand->argv[argc] = token;
        argc++;
        token = strtok(NULL, SEPARATORS);
    }
    comand->argv[argc] = '\0';
    comand->argc = argc;
    return 1;	
}

void readCommandLine(char *cmdLine){
    fgets(cmdLine, LINE_LEN, stdin);
    cmdLine[strlen(cmdLine)-1] = '\0';
    char clean[LINE_LEN]={};
    char * p = strtok(cmdLine, " ");
    while(p != NULL) {
      strcat(clean, p);
      strcat(clean, " \0");
      p = strtok(NULL, " ");
    }
    clean[strlen(clean)-1] = '\0';
    strcpy(cmdLine, clean);
}

int main(){
    tCommand command;
    char commandLine[LINE_LEN];
    for(int i=0; i<ARGS; i++)
        command.argv[i] = (char *) malloc(ARG_LEN);
    char path[PATH_LEN];
    getcwd(path, sizeof(path));
    while(1){
        printf("> "); 
        readCommandLine(commandLine);
        if (strlen(commandLine)!=0){
            if (strcmp(commandLine, "exit") == 0){
                    printf("Bye.\n");
                    exit(EXIT_SUCCESS);
            }
            getCommand(commandLine, &command);
            command.name  = getPath(command.argv[0], path);
            if(command.name == NULL){
                printf("Error: Command %s not found!\n", command.argv[0]);
            }else{
                if(fork() == 0){
                    execv(command.name, command.argv);
                }
                wait(NULL);
            }
        }      
        
  }
  exit(EXIT_SUCCESS);
}


