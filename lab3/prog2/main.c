#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    pid_t childpid[2];
    pid_t cpid, w;
    for (size_t i = 0; i < 2; i++)
    {
        childpid[i] = fork();
        if (childpid[i] == -1)
        {
            perror("Can't fork.\n");
            exit(1);
        }
        else if (childpid[i] == 0)
        {
            printf("Child process: ID = %d, PPID = %d, GROUP = %d\n", getpid(), getppid(), getpgrp());
            if (i == 0) {
                exit(0);
            } else {
                pause();
            }
        }
        else
        {
            printf("Parent process: PID = %d, GROUP = %d, child's ID = %d\n", getpid(), getpgrp(), childpid[i]);
       }
    }
    int status;
    for (int i = 0; i < 2; i++) {
        if (childpid[i] == -1)
        {
            perror("Can't wait.\n");
            exit(1);
        }
        cpid = wait(&status);
        printf("Child #%ld has finished: PID = %d, with status = %d\n", i + 1, cpid, status);
    }
    return 0;
}
