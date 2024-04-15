#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

void exit_handler(void) 
{
    printf("Обработчик выхода вызван, %d\n", getpid());
}

int main(int argc, char *argv[]) 
{
    int i;
    atexit(exit_handler);
    pid_t pid = fork();
    if (pid == 0) 
    {
        printf("Дочерний процесс - PID: %d, PPID: %d\n", getpid(), getppid());
        
        printf("Аргументы:\n");
        for (i = 0; i < argc; i++) 
        {
            printf("Аргумент [%d] - %s\n", i, argv[i]);
        }
    } 
    else if (pid > 0) 
    {
        printf("Родительский процесс - PID: %d, PPID: %d\n", getpid(), getppid());
        
        printf("Аргументы:\n");
        for (i = 0; i < argc; i++) 
        {
            printf("Аргумент [%d] - %s\n", i, argv[i]);
        }
        wait(NULL);
    } 
    else 
    {
        perror("fork");
        return 1;
    }
    return 0;
}