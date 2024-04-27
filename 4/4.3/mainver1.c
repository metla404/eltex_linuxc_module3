#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

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
    key_t key;
    
    int semid;
    struct sembuf sem_wait_writer = {0, -1, 0};
    struct sembuf sem_post_writer = {0, 1, 0};
    struct sembuf sem_wait_reader = {1, -1, 0};
    struct sembuf sem_post_reader = {1, 1, 0};

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

    key = ftok(".", 'a');
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    semid = semget(key, 2, IPC_CREAT | 0644);
    if (semid == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    semctl(semid, 0, SETVAL, 1); // Writer semaphore
    semctl(semid, 1, SETVAL, max_readers); // Reader semaphore

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    { // PARENT
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

            semop(semid, &sem_wait_writer, 1);
            fprintf(file, "%d\n", r);
            fflush(file);
            semop(semid, &sem_post_writer, 1);

            kill(pid, SIGUSR1);
            sleep(1);
            kill(pid, SIGUSR2);
        }

        fclose(file);
        close(fd[0]);
        wait(NULL);
    }
    else
    { // CHILD
        close(fd[0]); // read end of pipe

        signal(SIGUSR1, sig_handler);
        signal(SIGUSR2, sig_handler);

        srand(time(NULL));
        for (i = 0; i < num; i++)
        {
            int random_num = rand() % 100;
            write(fd[1], &random_num, sizeof(random_num));

            while (flag)
            {
                sleep(1);
            }
            sleep(1);

            semop(semid, &sem_wait_reader, 1);
            semop(semid, &sem_post_reader, 1);

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
            printf("Дочерний читает числа: %d\n", last_num);

            fclose(file);

            semop(semid, &sem_wait_reader, 1);
        }
        close(fd[1]);
    }
    semctl(semid, 0, IPC_RMID, 0);
    return 0;
}
