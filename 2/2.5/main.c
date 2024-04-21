#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

int main()
{
    int fd, counter = 0;
    char filename[] = "counter.txt";
    char buffer[10];

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        sprintf(buffer, "%d\n", counter);

        if (write(fd, buffer, strlen(buffer)) == -1)
        {
            perror("write");
            return 1;
        }
        counter++;
        sleep(1);
    }

    close(fd);
    return 0;
}
