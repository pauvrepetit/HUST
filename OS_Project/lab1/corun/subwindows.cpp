#include "subwindows.h"
#include <QApplication>

ShowTime *timeWin;
ShowFileContent *fileWin;
ShowAddCount *addWin;

int timeMain(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ShowTime st;
    timeWin = &st;
    st.show();
    return a.exec();
}

int fileMain(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ShowFileContent sfc;
    fileWin = &sfc;
    sfc.show();
    return a.exec();
}

int addMain(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ShowAddCount sac;
    addWin = &sac;
    sac.show();
    return a.exec();
}
