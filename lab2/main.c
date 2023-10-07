#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t childpid;
    if ((childpid = fork()) == -1) {
        perror("Cant fork\n");
        exit(1);
    }
    else if (childpid == 0) {
        while (1) printf("%d ", getpid());
        return 0;
    }
    else {
        while (1) printf("%d ", getpid());
        return 0;
    }

    return 0;
}
