#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <string.h>
#define LAST_MESSAGE 255
#define N 100
#define QUEUE_NAME "/myqueue1"
#define PRIORITY 1
#define SIZE 256

bool quit = false;

void handler(int signal)
{
    quit = true;
}

int main(int argc, char* argv[])
{
    pid_t pid;
    mqd_t que;
    
    char info[SIZE];
    struct mq_attr attr, old_attr;
    
    int priority;
    attr.mq_maxmsg = 32;
    attr.mq_msgsize = SIZE;

    if ((que = mq_open(QUEUE_NAME, O_CREAT | O_RDWR , 0777, &attr)) == (mqd_t)-1)
    {
        perror("create queue");
        exit(EXIT_FAILURE);
    }

    switch(pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            signal(SIGINT, handler);
            while(true)
            {
                if (quit) break;

                if (mq_receive(que, info, SIZE, &priority) == -1) continue;
            
                if(priority == 2) printf("[USER 2]: %s\n", info);
                
            }
            if (mq_close(que) == -1) perror("closing queue");
        
            if (mq_unlink(QUEUE_NAME) == -1) perror("removing queue");

            exit(EXIT_SUCCESS);
        
        default:
            pid_t ch_pid = pid;
            
            signal(SIGINT, handler);

            while(true)
            {
                priority = 1;
                if(quit) break;
                
                fgets(info, sizeof(info), stdin);

                if (mq_send(que, info, strlen(info), PRIORITY) == -1)
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }

            if (mq_close(que) == -1) perror("closing queue");

            exit(EXIT_SUCCESS);
            
    }
    exit(EXIT_SUCCESS);
} 