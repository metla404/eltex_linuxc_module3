#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#define FILE_NAME "numbers.txt"

void sigusr1_handler(int signum)
{
    printf("[CHILD] [PID: %d] [PPID: %d] Получен сигнал SIGUSR1, доступ к файлу заблокирован\n", getpid(), getppid());
}

void sigusr2_handler(int signum)
{
    printf("[CHILD] [PID: %d] [PPID: %d] Получен сигнал SIGUSR2, доступ к файлу разрешен\n", getpid(), getppid());
}

int main(int argc, char *argv[]) 
{
    int fd;
    FILE* f=fopen(FILE_NAME,"w");
    int buffer[10];

    int n, i;

    int pipefd[2];
    srand(time(0));

    if (argc == 1)
    {
        printf("Использование: %s <size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc != 2)
    {
        printf("Использование: %s <size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    if (pid > 0) // PARENT
    {
        close(pipefd[1]);

        int mas_receive[n];

        for (i = 0; i < n; i++)
        {
            read(pipefd[0], mas_receive, n * sizeof(int));
        }

        printf("\n[PARENT] [PID: %d] [PPID: %d]\nПолучил числа:\n", getpid(), getppid());
        for (i = 0; i < n; i++)
        {
            printf("%d ", mas_receive[i]);
            fprintf(f, "%d ", mas_receive[i]);
        }

        fclose(f);

        kill(pid, SIGUSR1);

        sleep(5);

        kill(pid, SIGUSR2);

        wait(NULL);
    }
    if (pid == 0) // CHILD
    {
        int mas_send[n];
        close(pipefd[0]);

        for (i=0; i < n; i++)
        {
            mas_send[i] = rand() % 101;
        }

        write(pipefd[1], mas_send, n * sizeof(int));

        printf("\n[CHILD] [PID: %d] [PPID: %d]\nCгенерировал и отправил числа:\n", getpid(), getppid());
        for(i = 0; i < n; i++)
        {
            printf("%d ", mas_send[i]);
        }
        printf("\n");

        struct sigaction sigusr1_action, sigusr2_action;
        memset(&sigusr1_action, 0, sizeof(sigusr1_action));
        memset(&sigusr2_action, 0, sizeof(sigusr2_action));
        sigusr1_action.sa_handler = sigusr1_handler;
        sigusr2_action.sa_handler = sigusr2_handler;
        sigaction(SIGUSR1, &sigusr1_action, NULL);
        sigaction(SIGUSR2, &sigusr2_action, NULL);

        // Читаем файл
        FILE* f=fopen(FILE_NAME,"r");
        for (i = 0; i < n; i++)
        {
            fscanf(f, "%d", &mas_send[i]);
            printf("[CHILD] [PID: %d] [PPID: %d] Прочитал число: %d\n", getpid(), getppid(), mas_send[i]);
        }
        fclose(f);

        mas_send[0] = rand() % 101;
        write(pipefd[1], mas_send, sizeof(int));
    }
    return 0;
}
