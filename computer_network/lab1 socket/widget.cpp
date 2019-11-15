#include <iostream>
#include <pthread.h>
#include "widget.h"
#include "ui_widget.h"
#include "mysocket.h"
#include "func.h"

pthread_t serverMainThread;
extern int serverSocket;


Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    ui->stopButton->setEnabled(false);
    // ui->log->document()->setMaximumBlockCount(1000);

    // set tab 切换顺序
    setTabOrder(ui->portNum, ui->maxConnNum);
    setTabOrder(ui->maxConnNum, ui->virtualPath);
    setTabOrder(ui->virtualPath, ui->listenAddr);


    connect(this, SIGNAL(sendMsg(QString)), this, SLOT(showMsg(QString)));
}

Widget::~Widget() { delete ui; }

void Widget::on_startButton_clicked()
{
    ui->stopButton->setEnabled(true);
    ui->startButton->setEnabled(false);
    ui->portNum->setEnabled(false);
    ui->maxConnNum->setEnabled(false);
    ui->virtualPath->setEnabled(false);
    ui->listenAddr->setEnabled(false);
    emit sendMsg("启动服务");
    // 创建参数结构体
    serverMainArg arg;
    arg.port = (unsigned short)(ui->portNum->value());
    arg.address = IPtoINT(ui->listenAddr->text());
    arg.listenQueueLen = ui->maxConnNum->value();
    arg.baseDir = ui->virtualPath->text();
    arg.widget = this;
    
    pthread_create(&serverMainThread, NULL, serverMain, (void *)&arg);
}

void Widget::on_stopButton_clicked()
{
    pthread_cancel(serverMainThread);
    pthread_join(serverMainThread, NULL);
    closeSocket(serverSocket);

    emit sendMsg("终止服务");

    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->portNum->setEnabled(true);
    ui->maxConnNum->setEnabled(true);
    ui->virtualPath->setEnabled(true);
    ui->listenAddr->setEnabled(true);
}

void Widget::showMsg(QString msg) {
    ui->log->append(msg);
    if(ui->log->toPlainText().count() > 10000) {
        ui->log->clear();
    }
    // 每10000个字符自动清空
}

void Widget::on_clearLogButton_clicked() {
    ui->log->clear();
}
