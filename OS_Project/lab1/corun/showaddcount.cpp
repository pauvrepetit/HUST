#include "showaddcount.h"
#include "ui_showaddcount.h"

ShowAddCount::ShowAddCount(QWidget *parent)
    : QDialog(parent), ui(new Ui::ShowAddCount) {
    ui->setupUi(this);
    this->clockCount = 0;
    this->sum = 0;
    connect(this, SIGNAL(clockSig()), this, SLOT(printCount()));
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
