#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int flag = 0;

void signal_handler(int signal) {
    flag = 1;
    printf("Catch: %d\n", signal);
}

int main(void)
{
    int fd[2];
    int status;
    pid_t childpid[2];
    pid_t child_pid;
    char buf[60];
    if (pipe(fd) == -1)
    {
        perror("Can't pipe.\n");
        exit(1);
    }
    char *const message[2] = {"aaa\n", "bbbbbbbbbbbbbbbb\n"};
    if (signal(SIGINT, signal_handler) == -1)
    {
    	printf("Can't signal.\n");
    	exit(1);
    }
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
            sleep(2);
            close(fd[0]);
            if (flag) {
                if (i == 0) {
                    sleep(2);
                }
                write(fd[1], message[i], strlen(message[i]));
                printf("Message sent to parent!\n");
            } else {
                printf("No message was sent\n");
            }
            exit(0);
        }
        else
        {
            printf("Parent process: PID = %d, GROUP = %d, child's ID = %d\n", getpid(), getpgrp(), childpid[i]);
        }
    }

    for (int i = 0; i < 2; i++)
    {
        child_pid = wait(&status);
        printf("Child has finished: PID = %d\n", child_pid);
        if (WIFEXITED(status))
            printf("Child exited with code %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Child terminated, recieved signal %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("Child stopped, recieved signal %d\n", WSTOPSIG(status));
        close(fd[1]);
        for (int i = 0; i < 60; i++)
        {
            buf[i] = '\0';
        }
        read(fd[0], buf, sizeof(buf));
        printf("Received messages: %s\n", buf);
    }
    close(fd[1]);
    for (int i = 0; i < 60; i++)
    {
        buf[i] = '\0';
    }
    read(fd[0], buf, sizeof(buf));
    printf("Received messages: %s\n", buf);
    return 0;
}
