#include <sys/types.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#define LAST_MESSAGE 255
#define N 100

int main(int argc, char* argv[])
{
    char key_name[] = "key"; 
    
    pid_t pid;
    key_t key; 
    
    int count,len, maxlen, msqid; 
    char input;

    srand(time(NULL));

    struct msg_buffer
    {
        long type;
        char info[N];
    } my_buf;

    if((key = ftok(key_name,0)) < 0)
    {
        printf("Не могу создать key\n");
        exit(EXIT_FAILURE);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Не могу получить msqid\n");
        exit(EXIT_FAILURE);
    }

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            while(true)
            {
                maxlen = 81;

                if(len = msgrcv(msqid, (struct msgbuf *) &my_buf, maxlen, 0, 0) < 0)
                {
                    printf("Ошибка получения сообщения из очереди\n");
                    exit(EXIT_FAILURE);
                }

                if(my_buf.type == LAST_MESSAGE)
                {
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_SUCCESS);
                }   

                if(my_buf.type == 2) printf("[USER 2]: %s\n", my_buf.info);
            }
            exit(EXIT_SUCCESS);
        
        default:
            pid_t ch_pid = pid;
            while(true)
            {                
                my_buf.type = 1;
                printf(">> ");
                fgets(my_buf.info, sizeof(my_buf.info), stdin);
        
                len = strlen(my_buf.info)+1;

                if (msgsnd(msqid, (struct msgbuf *) &my_buf, len, 0) < 0)
                {
                    printf("Ошибка отправки в очередь!\n");
                    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                    exit(EXIT_FAILURE);
                }
                printf("\nОтправлено: %s\n", my_buf.info);
            }
    }
    exit(EXIT_SUCCESS);
} 