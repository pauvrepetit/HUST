#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("argument error\n");
        printf("file_system <filename>\n");
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb+");
    if (fp == NULL) {
        printf("file %s not exist\n", argv[1]);
        return -1;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.setFp(fp);
    a.connect(&w, SIGNAL(exitProgram()), &a, SLOT(quit()));
    w.show();
    return a.exec();
}
