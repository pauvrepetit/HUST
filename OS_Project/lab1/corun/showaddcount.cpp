#include "showaddcount.h"
#include "ui_showaddcount.h"
#include <QTimer>

ShowAddCount::ShowAddCount(QWidget *parent)
    : QDialog(parent), ui(new Ui::ShowAddCount) {
    ui->setupUi(this);
    this->clockCount = 0;
    this->sum = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(printCount()));
    timer->start(100);
}

ShowAddCount::~ShowAddCount() { delete ui; }

void ShowAddCount::printCount() {
    char str[20];
    this->clockCount++;
    sprintf(str, "%d + %d = %d", this->sum, this->clockCount, this->sum + this->clockCount);
    this->sum += this->clockCount;
    QString qs(str);
    ui->textBrowser->append(qs);
    return;
}
