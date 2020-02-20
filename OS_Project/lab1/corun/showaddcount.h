#ifndef SHOWADDCOUNT_H
#define SHOWADDCOUNT_H

#include <QDialog>

namespace Ui {
class ShowAddCount;
}

class ShowAddCount : public QDialog {
    Q_OBJECT

  public:
    explicit ShowAddCount(QWidget *parent = nullptr);
    ~ShowAddCount();

  private slots:
    void printCount();

  private:
    int clockCount;
    int sum;
    Ui::ShowAddCount *ui;
};

#endif // SHOWADDCOUNT_H
