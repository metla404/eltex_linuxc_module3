#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int fd[2];
    pid_t pid;
    int i, num;

    if (argc != 2)
    {
        printf("Правило использования: ./main <кол-во_чисел>\n");
        return 1;
    }

    num = atoi(argv[1]);

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) // PARENT
    {
        close(fd[1]); // write end of pipe

        FILE *file = fopen("output.txt", "w");
        if (file == NULL)
        {
            printf("Ошибка открытия файла!\n");
            return 1;
        }

        for (i = 0; i < num; i++)
        {
            int r;
            read(fd[0], &r, sizeof(r));
            printf("Полученное число: %d\n", r);
            fprintf(file, "%d\n", r);
        }

        fclose(file);
        close(fd[0]);

    }
    else // CHILD
    { 
        close(fd[0]); // read end of pipe

        srand(time(NULL));
        for (i = 0; i < num; i++)
        {
            int random_num = rand() % 100;
            write(fd[1], &random_num, sizeof(random_num));
        }
        close(fd[1]);
    }
    return 0;
}
