#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define N 65536

void process_packet(unsigned char* buffer, int size, int *fd, char *p) 
{
    char data[N];
    
    struct iphdr *iph = (struct iphdr*)buffer;
    struct sockaddr_in source, dest;
    int port = atoi(p);
    
    bzero(&source, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    bzero(&dest, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    if(port == htons(source.sin_port) && port == htons(dest.sin_port))
    {
        printf("PACKET: %s -> %s\n", inet_ntoa(source.sin_addr), inet_ntoa(dest.sin_addr));
        sprintf(data, "PACKET: %s -> %s\n", inet_ntoa(source.sin_addr), inet_ntoa(dest.sin_addr));
        write(*fd, data, strlen(data)+1);
    }
}

int main(int argc, char* argv[]) 
{
    int sock, fd;
    struct sockaddr_in address;

    if(argc != 2)
    {
        perror("Использование: ./packet <порт>");
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = (unsigned char *)malloc(N);
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock < 0) 
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("bind error");
        exit(EXIT_FAILURE); 
    }

    fd = open("packets.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) 
    {
        perror("file error");
        exit(EXIT_FAILURE);
    }

    while(true) 
    {
        int packets;
        struct sockaddr saddr;
        socklen_t saddr_size = sizeof(saddr);
        packets = recvfrom(sock, buffer, N, 0, &saddr, &saddr_size);

        if(packets < 0) 
        {
            perror("recvfrom error");
            exit(EXIT_FAILURE); 
        }
        process_packet(buffer, packets, &fd, argv[1]);
    }

    free(buffer);
    close(sock);
    close(fd);
    exit(EXIT_SUCCESS);
}