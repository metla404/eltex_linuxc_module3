#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define SHM_SIZE 1024

volatile sig_atomic_t sets_processed = 0;

typedef struct
{
    int count;
    int num[0];
} dataset_s;

typedef struct
{
    int min;
    int max;
} result_s;

void handler_sigint(int signum)
{
    printf("SIGINT получен. Завершаю работу...\n");
    printf("\nОбработанных наборов данных: %d\n", sets_processed);
    exit(0);
}

int main()
{
    key_t key = ftok(".", 'a');
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    dataset_s *data = (dataset_s *) shmat(shmid, NULL, 0);
    result_s *result = (result_s *) (data + 1);

    signal(SIGINT, handler_sigint);

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        while (1)
        {
            // Ожидаем пока родитель не создаст 
            while (data->count == 0) {sleep(1);}

            int min = data->num[0];
            int max = data->num[0];
            for (int i = 1; i < data->count; i++)
            {
                if (data->num[i] < min)
                {
                    min = data->num[i];
                }
                if (data->num[i] > max)
                {
                    max = data->num[i];
                }
            }

            result->min = min;
            result->max = max;

            data->count = 0;

            sleep(1);
        }
    }
    else
    {
        srand(time(NULL));
        
        while (1)
        {
            data->count = rand() % 100;
            
            for (int i = 0; i < data->count; i++)
            {
                data->num[i] = rand() % 1000;
            }

            // Ждём, когда дочерний обработает
            while (result->min == 0 && result->max == 0) {sleep(1);}

            printf("Набор [%d]: MIN = %d, MAX = %d\n", sets_processed, result->min, result->max);

            result->min = 0;
            result->max = 0;
            sets_processed++;
            sleep(1);
        }
        wait(NULL);
    }
    return 0;
}
