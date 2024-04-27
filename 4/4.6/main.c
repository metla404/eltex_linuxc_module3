#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <semaphore.h>
#include <fcntl.h>

volatile sig_atomic_t flag = 0;

void sig_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        flag = 1;
    }
    else if (signo == SIGUSR2)
    {
        flag = 0;
    }
}

int main(int argc, char *argv[])
{
    int fd[2];
    pid_t pid;
    int i, num, max_readers;
    sem_t *writer_sem, *reader_sem;

    if (argc != 3)
    {
        printf("Правило использования: ./main <кол-во_чисел> <макс_кол-во_читателей>\n");
        return 1;
    }

    num = atoi(argv[1]);
    max_readers = atoi(argv[2]);

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    writer_sem = sem_open("writer_sem", O_CREAT, 0644, 1);
    reader_sem = sem_open("reader_sem", O_CREAT, 0644, max_readers);

    if (writer_sem == SEM_FAILED || reader_sem == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < max_readers; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            close(fd[0]); // read end of pipe

            signal(SIGUSR1, sig_handler);
            signal(SIGUSR2, sig_handler);

            srand(time(NULL) + getpid());

            for (int i = 0; i < max_readers; i++)
            {
                int random_num = rand() % 100;
                write(fd[1], &random_num, sizeof(random_num));

                while (flag)
                {
                    sleep(1);
                }
                sleep(1);

                sem_wait(reader_sem);

                FILE *file = fopen("output.txt", "r");
                if (file == NULL)
                {
                    printf("Ошибка открытия файла!\n");
                    return 1;
                }

                int last_num;
                for (int j = 0; j <= i; j++)
                {
                    if (j == i)
                    {
                        fscanf(file, "%d", &last_num);
                    }
                    else
                    {
                        fscanf(file, "%*d");
                    }
                }
                printf("[PID: %d] Дочерний читает числа: %d\n", getpid(), last_num);

                fclose(file);

                sem_post(reader_sem);
            }
            close(fd[1]);
            exit(0);
        }
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

            sem_wait(writer_sem);
            fprintf(file, "%d\n", r);
            fflush(file);
            sem_post(writer_sem);

            kill(pid, SIGUSR1);
            sleep(1);
            kill(pid, SIGUSR2);
        }

        fclose(file);
        close(fd[0]);

        for (int i = 0; i < max_readers; i++)
        {
            wait(NULL);
        }

        sem_close(writer_sem);
        sem_close(reader_sem);
        sem_unlink("writer_sem");
        sem_unlink("reader_sem");
    }

    return 0;
}
