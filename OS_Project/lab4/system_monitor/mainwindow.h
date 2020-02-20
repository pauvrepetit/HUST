#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  signals:
    void exit_program();

  private slots:
    void on_reboot_clicked();

    void on_shutdown_clicked();

    void on_exit_clicked();

    void updateTime();

    void updateWindow();

    void on_RefreshProc_clicked();

    void on_KillProc_clicked();

    void on_LoadModule_clicked();

    void on_UnloadModule_clicked();

    void on_newProcStart_clicked();

    void on_RefreshModule_clicked();

  private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
