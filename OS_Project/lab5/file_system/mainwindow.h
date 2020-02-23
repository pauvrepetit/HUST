#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int setFp(FILE *);

signals:
    void exitProgram();

private slots:
    void execCommand();

    void on_shell_textChanged();

    void on_writeClose();

private:
    Ui::MainWindow *ui;
    FILE *fp;
    long fileNodeOffset;
};
#endif // MAINWINDOW_H
