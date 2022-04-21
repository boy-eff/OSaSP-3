#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define child_amount 2

void print_time()
{
    struct timeval curTime;
    if (gettimeofday(&curTime, NULL) == -1)
    {
        perror("Get time error: \n");
        exit(-1);
    }
    int milli = curTime.tv_usec / 1000;

    char buffer [9];
    if (strftime(buffer, 9, "%H:%M:%S", localtime(&curTime.tv_sec)) == 0)
    {
        fprintf(stderr, "Array exciding error");
    }

    char currentTime[13] = "";
    if (sprintf(currentTime, "%s:%03d", buffer, milli) < 0)
    {
        fprintf(stderr, "Array exciding error");
    }
    printf("current time: %s \n", currentTime);
}

void print_process_info()
{
    printf("Child: %d Parent: %d \n", getpid(), getppid());
}

int main() {
    pid_t child_pid;
    printf("Parent: \n");
    print_time();
    print_process_info();
    printf("Children: \n");
    for (int i = 0; i < child_amount; i++)
    {
        child_pid = fork();

        switch (child_pid)
        {
            case 0:
            print_time();
            print_process_info();
            exit(0);
            case -1:
            perror("Fork error:\n");
            exit(-1);
            default:
            break;
        }
    }
    int error = 1; 
    while (error > 0)
    {
        error = wait(NULL) ;
    }
    if (error < 0 && errno != ECHILD)
    {
        perror("Wait error:");
    }
    return 0;
    
}