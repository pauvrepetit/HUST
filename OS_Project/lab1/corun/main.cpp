#include "subwindows.h"
#include <QApplication>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

pid_t timePid, addPid, filePid;
extern ShowTime *timeWin;
extern ShowFileContent *fileWin;
extern ShowAddCount *addWin;

void signalTime(int sigNum) {
    if(sigNum == SIGUSR1) {
        emit timeWin->clockSig();
    }
    return;
}

void signalAdd(int sigNum) {
    if(sigNum == SIGUSR1) {
        emit addWin->clockSig();
    }
    return;
}

void signalFile(int sigNum) {
    if(sigNum == SIGUSR1) {
        emit fileWin->clockSig();
    }
    return;
}

int main(int argc, char *argv[]) {
    timePid = fork();
    if (timePid == 0) {
        // 输出时间的进程
        signal(SIGUSR1, signalTime);
        return timeMain(argc, argv);
    }

    addPid = fork();
    if (addPid == 0) {
        // 输出累加结果的进程
        signal(SIGUSR1, signalAdd);
        return addMain(argc, argv);
    }

    filePid = fork();
    if (filePid == 0) {
        // 输出文件内容的进程
        signal(SIGUSR1, signalFile);
        return fileMain(argc, argv);
    }

    sleep(1);

    for(int i = 0; i < 100; i++) {
        kill(timePid, SIGUSR1);
        kill(filePid, SIGUSR1);
        kill(addPid, SIGUSR1);
        sleep(1);
    }

    waitpid(timePid, NULL, 0);
    waitpid(addPid, NULL, 0);
    waitpid(filePid, NULL, 0);

    return 0;
}
