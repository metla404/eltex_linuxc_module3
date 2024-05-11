#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>


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
    int sockfd, newsockfd, portno, max_sd, activity, sd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    fd_set readfds;

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

    max_sd = sockfd;

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(sockfd, &readfds);
        max_sd = sockfd;

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
            error("select error");

        if (FD_ISSET(sockfd, &readfds))
        {
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

            if (newsockfd < 0)
                error("accept error");

            FD_SET(newsockfd, &readfds);

            if (newsockfd > max_sd)
                max_sd = newsockfd;

            printf("Новое соединение: %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        }

        for (sd = 0; sd <= max_sd; sd++)
        {
            if (FD_ISSET(sd, &readfds))
            {
                if (sd == sockfd)
                {
                    continue;
                }

                dostuff(sd);

                close(sd);

                FD_CLR(sd, &readfds);
            }
        }
    }

    close(sockfd);
    return 0;
}
