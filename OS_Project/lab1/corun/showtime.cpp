#include "showtime.h"
#include "ui_showtime.h"
#include <QTimer>

ShowTime::ShowTime(QWidget *parent) : QDialog(parent), ui(new Ui::ShowTime) {
    ui->setupUi(this);
    ui->Time->setEnabled(false);
    ui->Time->setDateTime(QDateTime::currentDateTime());

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(printTime()));
    timer->start(1000);
}

ShowTime::~ShowTime() { delete ui; }

void ShowTime::printTime() {
    ui->Time->setDateTime(QDateTime::currentDateTime());
    return;
}
