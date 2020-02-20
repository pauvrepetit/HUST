#ifndef SHOWTIME_H
#define SHOWTIME_H

#include <QDialog>

namespace Ui {
class ShowTime;
}

class ShowTime : public QDialog {
    Q_OBJECT

  public:
    explicit ShowTime(QWidget *parent = nullptr);
    ~ShowTime();

private slots:
    void printTime();

private:
    Ui::ShowTime *ui;
};

#endif // SHOWTIME_H
