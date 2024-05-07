#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
    socklen_t clilen1 = sizeof(cli_addr1);
    socklen_t clilen2 = sizeof(cli_addr2);
    char buffer[BUF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr1, 0, sizeof(cli_addr1));
    memset(&cli_addr2, 0, sizeof(cli_addr2));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("[Сервер запущен...]\n");

    while (1)
    {
        if (recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr1, &clilen1) > 0)
        {
            printf("Клиент [1]: %s\n", buffer);
            sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&cli_addr2, clilen2);
        }

        if (recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr2, &clilen2) > 0)
        {
            printf("Клиент [2]: %s\n", buffer);
            sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&cli_addr1, clilen1);
        }
    }
    return 0;
}