#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAX_ARGS 10
#define PATH_DELIM ":"

void exe_com(char **args)
{
    pid_t pid;
    int status;
    pid = fork();

    if (pid == 0) 
    {
        if (execvp(args[0], args) == -1) 
        {
            perror("execvp");
            exit(1);
        }
    } 
    else if (pid > 0) 
    {
        wait(&status);
    } 
    else 
    {
        perror("fork");
        exit(1);
    }
}

int main()
{
    char command[1024];
    char *args[MAX_ARGS];
    int i;

    while (1)
    {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);

        command[strlen(command) - 1] = '\0';

        i = 0;
        args[i] = strtok(command, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }
        exe_com(args);
    }

    return 0;
}
