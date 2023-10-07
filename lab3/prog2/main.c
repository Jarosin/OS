#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define N 2

int main(void)
{
    pid_t childpid[N];
    pid_t cpid, w;
    for (size_t i = 0; i < N; i++)
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
            exit(0);
        }
        else
        {
            printf("Parent process: PID = %d, GROUP = %d, child's ID = %d\n", getpid(), getpgrp(), childpid[i]);
       }
    }
    int status;
    for (int i = 0; i < N; i++) {
        if (childpid[i] == -1)
        {
            perror("Can't wait.\n");
            exit(1);
        }
        cpid = wait(&status);
        printf("Child #%ld has finished: PID = %d\n", i + 1, cpid);
        if (WIFEXITED(status))
            printf("Child #%ld exited with code %d\n", i + 1, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Child #%ld terminated, recieved signal %d\n", i + 1, WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("Child #%ld stopped, recieved signal %d\n", i + 1, WSTOPSIG(status));
    }
    return 0;
}