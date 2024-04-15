#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int i, n, half;
    double side, area, total_half1, total_half2, total_all = 0;

    if (argc < 2)
    {
        printf("Неверное количество аргументов!\n", argv[0]);
        return 1;
    }

    n = argc - 1;
    half = n / 2;

    pid_t pid = fork();

    if (pid == 0)
    {
        printf("\nДоч. процесс - PID: %d, PPID: %d\n", getpid(), getppid());
        for (i = 0; i < half; i++)
        {
            side = atof(argv[i + 1]);
            area = pow(side, 2);
            printf("Площадь квадрата %d = %.2f\n", i + 1, area);
            
            total_half1 += area;
            if (i == half-1)
            {
                printf("Суммарная площадь квадратов (первая половина аргументов) род. процесса: %.2f\n", total_half1);
                total_all = total_all + total_half1;
            }
        }

    } 
    else if (pid > 0)
    {
        printf("\nРод. процесс - PID: %d, PPID: %d\n", getpid(), getppid());
        for (i = half; i < n; i++)
        {
            side = atof(argv[i + 1]);
            area = pow(side, 2);
            printf("Площадь квадрата %d = %.2f\n", i + 1, area);
            
            total_half2 += area;
            if (i == n-1)
            {
                printf("Суммарная площадь квадратов (вторая половина аргументов) род. процесса: %.2f\n", total_half2);
                total_all = total_all + total_half2;
            }
            
        }
        wait(NULL);
    }
    else 
    {
        perror("fork");
        return 1;
    }
    return 0;
}
