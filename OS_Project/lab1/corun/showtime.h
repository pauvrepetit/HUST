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

  signals:
    void clockSig();

private slots:
    void printTime();

private:
    Ui::ShowTime *ui;
};

#endif // SHOWTIME_H
