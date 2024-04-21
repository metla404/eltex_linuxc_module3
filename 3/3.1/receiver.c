#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LAST_MESSAGE 255

int main()
{
    pid_t pid;    
    int msqid; 
    
    key_t key; 
    int len, maxlen; 

    char pathname[] = "key";

    struct msg_buffer
    {
        long type;
        char info[81];
    } msg_buf;

    
    if((key = ftok(pathname,0)) < 0)
    {
        printf("Не удается сгенерировать ключ\n");
        exit(EXIT_FAILURE);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Не удается получить msqid\n");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        maxlen = 81;

        if((len = msgrcv(msqid, (struct msgbuf *) &msg_buf, maxlen, 0, 0)) < 0)
        {
            printf("Не удается получить сообщение из очереди\n");
            exit(EXIT_FAILURE);
        }

        if(msg_buf.type == LAST_MESSAGE)
        {
            printf("\n\nПолучено сообщение\n[TYPE: %ld], [INFO: %s]\n\nЗавершаю работу...\n", msg_buf.type, msg_buf.info);
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(EXIT_SUCCESS);
        } 

        printf("[TYPE: %ld], [INFO: %s]\n", msg_buf.type, msg_buf.info);
    }
} 