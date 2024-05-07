#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buff[1024];
    char op;
    
    int num1, num2;

    if (argc < 3)
    {
        fprintf(stderr, "Использование: %s <хост> <порт>\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) 
        error("socket error");

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Такого хоста нет.\n");
        exit(0);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    bcopy((char*)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(my_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("connect error");

    printf("Введите одну из четырех операций [ +  -  *  / ]: ");
    scanf(" %c", &op);

    send(my_sock, &op, 1, 0);

    printf("Число 1: ");
    scanf("%d", &num1);
    send(my_sock, &num1, sizeof(num1), 0);

    printf("Число 2: ");
    scanf("%d", &num2);
    send(my_sock, &num2, sizeof(num2), 0);

    n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0);
    buff[n] = 0;
    printf("Результат : %s", buff);

    close(my_sock);
    return 0;
}
