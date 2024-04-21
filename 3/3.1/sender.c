#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LAST_MESSAGE 255

struct msg_buffer
{
    long type;
    char info[81];
} msg_buf;

volatile sig_atomic_t stop = 0;

void handler(int sig)
{
    stop = 1;
}

int main(int argc, char *argv[])
{
    int msqid, len;
    key_t key;
    pid_t pid;

    char pathname[] = "key";

    srand(time(NULL));

    if ((key = ftok(pathname, 0)) == -1)
    {
        perror("Ошибка при создании ключа");
        exit(EXIT_FAILURE);
    }

    if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1)
    {
        perror("Ошибка при получении идентификатора очереди сообщений");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handler);

    switch (pid = fork())
    {
    case -1:
        perror("Ошибка при разделении процесса");
        exit(EXIT_FAILURE);

    case 0:
        while (!stop)
        {
            msg_buf.type = rand() % 10 + 250;
            sprintf(msg_buf.info, "%d", rand() % 10);
            len = strlen(msg_buf.info) + 1;

            if (msgsnd(msqid, &msg_buf, len, 0) == -1)
            {
                perror("Ошибка при отправке сообщения в очередь");
                msgctl(msqid, IPC_RMID, NULL);
                exit(EXIT_FAILURE);
            }
            sleep(1);
        }

        msg_buf.type = LAST_MESSAGE;
        len = 0;

        if (msgsnd(msqid, &msg_buf, len, 0) == -1)
        {
            perror("Ошибка при отправке сообщения в очередь");
            msgctl(msqid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }
        break;

    default:
        pause();
        kill(pid, SIGINT);
        msgctl(msqid, IPC_RMID, NULL);
        break;
    }
    exit(EXIT_SUCCESS);
}
