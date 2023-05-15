#include "myshell.h"

int main(int argc, char *argv[]){
    envSet();
    
    //runs batch file
    if(argv[1]){
        //run batch instead
    }

    while(1){
        char* line = NULL;
        size_t len = 0;
        int oldStdout, oldStdin;
        int newStdout, newStdin;
        int needPipe;
        //gets the current directory
        char cwd[200];
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s> ", getenv("HOME"));
        }

        //gets the user input
        getline(&line, &len, stdin);
        

        //tokenizing the line
        int argc2, count;
        char **argv2 = malloc(BUFSIZ * sizeof(char *));

        if(argv2 == NULL){
            perror("error creating tokenized line");
            exit(1);
        }

        char* token = strtok(line, "  \t\r\n\a");
        while(token != NULL){
            argv2[count] = token;
            argc2++;
            count++;
            token = strtok(NULL, "  \t\r\n\a");
        }
        argv2[count] = NULL;//end value is NULL for external cmds

        //if command is built in or external
        int isBuiltin = ifInCmd(argv2);

        //if there is piping
        if((needPipe = ifPipe(argv2)) != -1){
            if(argc2 <= 2){
                perror("cannot pipe one command");
            }

            int toPipe[2];//creates the new pipe
            if(pipe(toPipe) == -1){
                perror("pipe failed");
                exit(-1);
            }
            //creates the fork
            pid_t pid = fork();

            if(pid == 0){//if child

                //take write portion of pipe and make it stdin for 2nd cmd
                if((dup2(toPipe[0], STDIN_FILENO)) == -1){
                    perror("error replaceing stdout with pipe");
                    exit(1);
                }

                //runs first command
                execvp(argv2[needPipe - 1], argv2);
            }
            else{//parent
                
                //take read portion of pipe and makes it stdout for 1st cmd
                if((dup2(toPipe[1], STDOUT_FILENO)) == -1){
                    perror("error replaceing stdin with pipe");
                    exit(1);
                }

                //second cmd
                int status;
                waitpid(pid, &status, 0);
                execvp(argv2[needPipe + 1], argv2);
                
            }
            //closes pipes
            close(toPipe[1]);
            close(toPipe[0]);

        }

        if(isBuiltin != -1){// bulit in commands
            //checks to see if there is input or output redirection
            int isIn = ifInRedi(argv2);
            int isOut = ifOutRedi(argv2);

            //printf("%d %d\n", isIn, isOut);

            if(isIn != -1){// if the input is redirected  

                oldStdin = dup(0); //copy current stdin and redirect stdin
                newStdin = open(argv2[isIn], O_RDONLY);

                dup2(newStdin, 0);//put the redirected stdin 
                close(newStdin);
            }

            if(isOut != -1){//if the output is redirected

                oldStdout = dup(1);//copy current stdout and redirect stdout
                newStdout = open(argv2[isOut], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

                dup2(newStdout, 1);//put the redirected stout 
                close(newStdout);
            }
            
            runBulitIn(isBuiltin, argv2);//runs the bulit in commands 

            if(isIn != -1){
            //returns the original stdin and closes it
                dup2(oldStdin, 0);
                close(oldStdin);
            }

            if(isOut != -1){
            //returns the original stdout and closes it
                dup2(oldStdout, 1);
                close(oldStdout);
            }
            
        }
        else{//eternal command
            if(needPipe == -1){
                int isIn = ifInRedi(argv2);
                int isOut = ifOutRedi(argv2);

                if(isIn != -1){// if the input is redirected  

                    oldStdin = dup(0); //copy current stdin and redirect stdin
                    newStdin = open(argv2[isIn], O_RDONLY);

                    dup2(newStdin, 0);//put the redirected stdin 
                    close(newStdin);
                }

                if(isOut != -1){//if the output is redirected

                    oldStdout = dup(1);//copy current stdout and redirect stdout
                    newStdout = open(argv2[isOut], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

                    dup2(newStdout, 1);//put the redirected stout 
                    close(newStdout);
                }

                //fork for external cmds
                pid_t pid = fork();

                if(pid == 0){//child runs function
                    execvp(argv2[0], argv2);
                }
                else{ //parent waits to child to finish
                    if(ifBg(argv2) == -1){
                        int status = 0;
                        waitpid(pid, &status, 0);
                    }
                    
                }


                if(isIn != -1){
                    //returns the original stdin and closes it
                    dup2(oldStdin, 0);
                    close(oldStdin);
                }

                if(isOut != -1){
                    //returns the original stdout and closes it
                    dup2(oldStdout, 1);
                    close(oldStdout);
                }
            }
            

        }
    
        free(line);
        free(argv2);
    }
}

void sCd(char** arg){
    //if just cd typed return cur dr
    if(arg[1] == NULL){
        printf("%s\n", getenv("HOME"));
    }
    else{//change the dir
        if(chdir(arg[1]) != 0){
            perror("error changing directories");
        }
    }
}

void sClr(){
    printf("\e[1;1H\e[2J");//print statement to clear screen
}

void sDir(char** arg){
    char* toOpen = arg[1];
    //opens the curr dir to be read
    DIR *direc;
    struct dirent *file;
    if(arg[1] == NULL){
        perror("no dir specificed");
        return;
    }
    direc = opendir(toOpen);
    //if there was an error
    if(direc == NULL){
        perror("ERROR\n");
        return;
    }
    //prints each file within dir
    while((file = readdir(direc)) != NULL){
        printf("%s\n", file->d_name);
    }
    closedir(direc);
}

void sEnviron(){
    int i;

    //prints each environ variable
    while(environ[i]){
        printf("%s\n", environ[i]);
        i++;
    }
}

void sEcho(char** arg){
    int i = 1;

    //prints each string typed after echo
    while(arg[i] != NULL){
        printf("%s ", arg[i]);
        i++;
    }
    printf("\n");
}

void sHelp(){
    FILE *fp = fopen("readme_doc", "r");
    if(fp == NULL){
        perror("cannot read file");
        return;
    }

    char line[200];

    while(fgets(line, 128, fp)){
        printf("%s ", line);
    }
    printf("\n");
    fclose(fp);
}

void sPause(){
    while(getchar() != '\n'){}//waits for the enter char before moving on
}

void sQuit(){
    exit(0);//ends the program
}

int ifInCmd(char** arg){
    //checks for the internal commands and returns which command is called
    if(strcmp(arg[0], "cd") == 0){
        return 0;
    }
    else if(strcmp(arg[0], "clr") == 0){
        return 1;
    }
    else if(strcmp(arg[0], "dir") == 0){
        return 2;
    }
    else if(strcmp(arg[0], "environ") == 0){
        return 3;
    }
    else if(strcmp(arg[0], "echo") == 0){
        return 4;
    }
    else if(strcmp(arg[0], "help") == 0){
        return 5;
    }
    else if(strcmp(arg[0], "pause") == 0){
        return 6;
    }
    else if(strcmp(arg[0], "quit") == 0 || strcmp(arg[0], "exit") == 0){
        return 7;
    }

    return -1;
}

int ifOutRedi(char** arg){
    int i = 0;

    while(arg[i]){ //returns the value of the file to redirect to
        if(strcmp(arg[i], ">") == 0 || strcmp(arg[i], ">>") == 0){
            return i + 1;
        }
        i++;
    }
    return -1;
}

int ifInRedi(char** arg){
    int i = 0;

    while(arg[i]){//returns the value of the file to redirect to
        if(strcmp(arg[i], "<") == 0){
            return i + 1;
        }
        i++;
    }
    return -1;
}

int ifPipe(char** arg){
    int i = 0;

    while(arg[i] != NULL){// returns location of a pipe
        if(strcmp(arg[i], "|") == 0){
            return i;
        }
        i++;
    }
    return -1;
}

int ifBg(char** arg){
    int i = 0;

    while(arg[i]){//returns if there is background processing
        if(strcmp(arg[i], "&") == 0){
            return i;
        }
        i++;
    }
    return -1;
}

void runBulitIn(int bulitIn, char** arg){
    //runs the builtin commands
    if(bulitIn == 0){
        sCd(arg);
    }
    else if(bulitIn == 1){
        sClr();
    }
    else if(bulitIn == 2){
        sDir(arg);
    }
    else if(bulitIn == 3){
        sEnviron();
    }
    else if(bulitIn == 4){
        sEcho(arg);
    }
    else if(bulitIn == 5){
        sHelp();
    }
    else if(bulitIn == 6){
        sPause();
    }
    else if(bulitIn ==7){
        sQuit();
    }
}

void envSet(){
    setenv("HOME", "myshell", 1);
}





