#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

  public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

  signals:  // 自定义信号
    void sendMsg(QString);

  private slots:    // 槽函数
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void showMsg(QString);

    void on_clearLogButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
