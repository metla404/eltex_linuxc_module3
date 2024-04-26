#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int fd1[2], fd2[2];
    pid_t pid;
    int i, num;

    if (argc != 2)
    {
        printf("Правило использования: ./main <кол-во_чисел>\n");
        return 1;
    }

    num = atoi(argv[1]);

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
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
        close(fd1[1]); // close write end of pipe1
        close(fd2[0]); // close read end of pipe2

        FILE *file = fopen("output.txt", "w");
        if (file == NULL)
        {
            printf("Ошибка открытия файла!\n");
            return 1;
        }

        for (i = 0; i < num; i++)
        {
            int r;
            read(fd1[0], &r, sizeof(r));
            printf("Полученное число: %d\n", r);
            fprintf(file, "%d\n", r);

            int response = r * 2;
            write(fd2[1], &response, sizeof(response));
        }

        fclose(file);
        close(fd1[0]);
        close(fd2[1]);

    }
    else // CHILD
    {
        close(fd1[0]); // close read end of pipe1
        close(fd2[1]); // close write end of pipe2

        srand(time(NULL));
        for (i = 0; i < num; i++)
        {
            int random_num = rand() % 100;
            write(fd1[1], &random_num, sizeof(random_num));

            int parent_response;
            read(fd2[0], &parent_response, sizeof(parent_response));
            printf("Дочерний получил: %d\n", parent_response);
        }
        close(fd1[1]);
        close(fd2[0]);
    }
    return 0;
}
