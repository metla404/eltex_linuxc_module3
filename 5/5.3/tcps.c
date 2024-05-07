#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int myfunc(int a, int b, char op)
{
    switch (op)
    {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            if (b == 0)
            {
                printf("Деление на ноль!\n");
                return 0;
            }
            return a / b;
        default:
            printf("Ошибка! Неопределенная операция!\n");
            return 0;
    }
}

void dostuff(int sock)
{
    int bytes_recv;
    char buff[20 * 1024];
    char op;
    int a, b;

    bytes_recv = recv(sock, &op, 1, 0);
    if (bytes_recv < 0) 
        error("socket read error");

    bytes_recv = recv(sock, &a, sizeof(a), 0);
    if (bytes_recv < 0) 
        error("socket read error");

    bytes_recv = recv(sock, &b, sizeof(b), 0);
    if (bytes_recv < 0) 
        error("socket read error");

    a = myfunc(a, b, op);

    snprintf(buff, sizeof(buff), "%d", a);
    buff[strlen(buff)] = '\n';

    send(sock, &buff[0], strlen(buff), 0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("socket error");

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("bind error");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newsockfd < 0) 
            error("accept error");
        pid = fork();
        
        if (pid < 0) 
            error("fork error");
        if (pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        } 
        else
        {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
