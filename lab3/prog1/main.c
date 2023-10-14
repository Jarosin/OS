#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t childpid[2];
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
            printf("Child process before sleep: ID = %d, PPID = %d, GROUP = %d\n", getpid(), getppid(), getpgrp());
	        sleep(4);
	        printf("\nChild process after sleep: ID = %d, PPID = %d, GROUP = %d\n", getpid(), getppid(), getpgrp());
	        exit(0);
	    }
	    else
	    {
	        printf("Parent process: PID = %d, GROUP = %d\n",getpid(),getpgrp());
	    }
    }
    return 0;
}
