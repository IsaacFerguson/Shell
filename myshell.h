#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>



extern char **environ;


void sCd(char**);
void sClr();
void sDir(char**);
void sEnviron();
void sEcho(char**);
void sHelp();
void sPause();
void sQuit();
int ifInCmd(char**);
int ifOutRedi(char**);
int ifInRedi(char**);
int ifPipe(char**);
int ifBg(char**);
void runBulitIn(int, char**);
void envSet();
