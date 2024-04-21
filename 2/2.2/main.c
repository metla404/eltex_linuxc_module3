#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int n, i;
    int *arr;

    if (argc != 2)
    {
        printf("Использование: %s <size>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);
    if (n <= 0)
    {
        printf("Размер массива должен быть положительным!\n");
        return 1;
    }
    
    arr = malloc(n * sizeof(int));

    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        perror("pipe");
        free(arr);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(arr);
        return 1;
    }

    if (pid == 0)
    {
        close(fd1[0]);
        close(fd2[1]);

        printf("[FD1] [CHILD] Генерирует и отправляет массив...\n");
        for (i = 0; i < n; i++)
        {
            arr[i] = rand() % 100;
            printf("%d ", arr[i]);
        }

        printf("\n");

        if (write(fd1[1], arr, n * sizeof(int)) == -1)
        {
            perror("write");
            free(arr);
            return 1;
        }

        printf("[FD2] [CHILD] Читает полученный ответ от [PARENT]\n");
        if (read(fd2[0], arr, n * sizeof(int)) == -1)
        {
            perror("read");
            free(arr);
            return 1;
        }

        printf("[FD2] [CHILD] Получил: ");
        for (i = 0; i < n; i++)
        {
            printf("%d ", arr[i]);
        }
        printf("\n");

        close(fd1[1]);
        close(fd2[0]);

        free(arr);
        return 0;
    }
    else
    {
        sleep(1);

        close(fd1[1]);
        close(fd2[0]);

        printf("[FD1] [PARENT] Читает массив от [CHILD]...\n");
        if (read(fd1[0], arr, n * sizeof(int)) == -1)
        {
            perror("read");
            free(arr);
            return 1;
        }

        for (i = 0; i < n; i++)
        {
            arr[i] *= 2;
        }
        
        printf("[FD2] [PARENT] Отправляет ответ - модифицирует числа...\n");
        if (write(fd2[1], arr, n * sizeof(int)) == -1)
        {
            perror("write");
            free(arr);
            return 1;
        }

        close(fd1[0]);
        close(fd2[1]);

        wait(NULL);

        free(arr);
        return 0;
    }
}
