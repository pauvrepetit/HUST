#include "showfilecontent.h"
#include "ui_showfilecontent.h"

ShowFileContent::ShowFileContent(QWidget *parent)
    : QDialog(parent), ui(new Ui::ShowFileContent) {
    ui->setupUi(this);
    fp.open("/etc/fstab", std::ios::in);
    connect(this, SIGNAL(clockSig()), this, SLOT(printFile()));
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
