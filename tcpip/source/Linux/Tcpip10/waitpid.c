#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int status;
    pid_t pid = fork();

    if (pid == 0) {
        sleep(15);
        return 24;
    } else {
        while (!waitpid(-1, &status, WNOHANG)) {
            sleep(3);
            puts("sleep 3 sec");
        }

        if (WIFEXITED(status)) {
            printf("child send %d \n", WEXITSTATUS(status));
        }
    }

    return 0;
}