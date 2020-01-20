#include "showtime.h"
#include "ui_showtime.h"

ShowTime::ShowTime(QWidget *parent) : QDialog(parent), ui(new Ui::ShowTime) {
    ui->setupUi(this);
    connect(this, SIGNAL(clockSig()), this, SLOT(printTime()));
}

ShowTime::~ShowTime() { delete ui; }

void ShowTime::printTime() {
    time_t nowTime = time(NULL);
    char *time_str = ctime(&nowTime);
    int i = 0;
    while (time_str[i] != '\n') {
        i++;
    }
    time_str[i] = '\0';
    QString qs(time_str);
    ui->textBrowser->append(qs);
    return;
}
