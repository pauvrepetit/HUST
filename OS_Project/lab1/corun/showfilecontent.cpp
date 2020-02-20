#include "showfilecontent.h"
#include "ui_showfilecontent.h"
#include <QTimer>

ShowFileContent::ShowFileContent(QWidget *parent)
    : QDialog(parent), ui(new Ui::ShowFileContent) {
    ui->setupUi(this);
    fp.open("/etc/fstab", std::ios::in);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(printFile()));
    timer->start(1000);
}

ShowFileContent::~ShowFileContent() {
    fp.close();
    delete ui;
}

void ShowFileContent::printFile() {
    std::string str;
    std::getline(fp, str);
    if (str[0] != 0) {
        QString qs = QString::fromStdString(str);
        ui->textBrowser->append(qs);
    }
    return;
}
