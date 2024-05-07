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
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(serv_addr);
    char buffer[BUF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    while (1)
    {
        fgets(buffer, BUF_SIZE, stdin);

        if (sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, addrlen) < 0)
        {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }

        if (recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *)&serv_addr, &addrlen) < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        printf("Сообщение: %s\n", buffer);
    }
    return 0;
}