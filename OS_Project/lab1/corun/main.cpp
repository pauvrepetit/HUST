#include "subwindows.h"
#include <QApplication>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

pid_t timePid, addPid, filePid;
extern ShowTime *timeWin;
extern ShowFileContent *fileWin;
extern ShowAddCount *addWin;

int main(int argc, char *argv[]) {
    timePid = fork();
    if (timePid == 0) {
        // 输出时间的进程
        return timeMain(argc, argv);
    }

    addPid = fork();
    if (addPid == 0) {
        // 输出累加结果的进程
        return addMain(argc, argv);
    }

    filePid = fork();
    if (filePid == 0) {
        // 输出文件内容的进程
        return fileMain(argc, argv);
    }

    waitpid(timePid, NULL, 0);
    waitpid(addPid, NULL, 0);
    waitpid(filePid, NULL, 0);

    return 0;
}
