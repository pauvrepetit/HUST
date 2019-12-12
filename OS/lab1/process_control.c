#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

__pid_t p1;
__pid_t p2;
int pipefd[2];

void sig_func(int sig_no) {
    if (sig_no == SIGINT) {
        if (kill(p1, SIGUSR1) == 1) {
            printf("In parent process, send SIGUSR1 to p1 failed\n");
        }
        if (kill(p2, SIGUSR2) == 1) {
            printf("In parent process, send SIGUSR2 to p2 failed\n");
        }
    } else if (sig_no == SIGUSR1) {
        close(pipefd[1]);
        printf("Child Process 1 is Killed by Parent!\n");
        exit(0);
    } else if (sig_no == SIGUSR2) {
        close(pipefd[0]);
        printf("Child Process 2 is Killed by Parent!\n");
        exit(0);
    } else {
        printf("unknown signal\n");
    }
}

int main() {
    if (pipe(pipefd) == -1) {
        printf("pipe create error\n");
        return 0;
    }

    p1 = fork();
    if (p1 == -1) {
        printf("fork p1 error\n");
        return 0;
    }
    if (p1 != 0) {
        // main
        p2 = fork();
        if (p2 == -1) {
            printf("fork p2 error\n");
            kill(p1, SIGUSR1);
            waitpid(p1, NULL, 0);
            return 0;
        }
        if (p2 != 0) {
            // main
            if (signal(SIGINT, sig_func) == SIG_ERR) {
                printf("In parent process, ignore SIGINT failed\n");
            }
        } else {
            // process 2
            if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
                printf("In child 2 process, ignore SIGINT failed\n");
            }
            if (signal(SIGUSR2, sig_func) == SIG_ERR) {
                printf("In child 2 process, set SIGUSR2 failed\n");
            }
            char str[30];
            while (1) {
                read(pipefd[0], str, 30);
                printf("%s", str);
            }
        }
    } else {
        // process 1
        if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
            printf("In child 1 process, ignore SIGINT failed\n");
        }
        if (signal(SIGUSR1, sig_func) == SIG_ERR) {
            printf("In child 1 process, set SIGUSR1 failed\n");
        }
        char str[30];
        int times = 1;
        while (1) {
            sprintf(str, "I send you %d times.\n", times++);
            write(pipefd[1], str, 30);
            sleep(1);
        }
    }

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    printf("Parent Process is Killed\n");
    exit(0);
}