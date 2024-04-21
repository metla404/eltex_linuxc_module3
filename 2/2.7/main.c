#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_count = 0;

void sigint_handler(int signum) 
{
    if (signum == SIGQUIT)
    {
        printf("\n\nПолучен сигнал SIGINT!\n");
    }
    if (signum == SIGINT)
    {
        printf("\n\nПолучен сигнал SIGINT!\n");
    }
}

int main()
{
    int fd, counter = 0;

    char filename[] = "counter.txt";
    char buffer[10];

    if (signal(SIGINT, &sigint_handler) == SIG_ERR || signal(SIGQUIT, &sigint_handler) == SIG_ERR)
    {
        perror("handler");
        return EXIT_FAILURE;
    }

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        sprintf(buffer, "%d\n", counter);

        if (write(fd, buffer, strlen(buffer)) == -1)
        {
            perror("write");
            return 1;
        }
        counter++;
        sleep(1);
    }

    close(fd);
    return 0;
}
