#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fs.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->WriteClose->setEnabled(false);

    connect(ui->commandLine, SIGNAL(returnPressed()), this, SLOT(execCommand()));
    connect(ui->WriteClose, SIGNAL(pressed()), this, SLOT(on_writeClose()));

    QString init("if you first use the file system, please format it please\n"
                 "input 'format' to format the file system\n"
                 "input 'help' for more information\n"
                 "Warning: you have to use only one space between words in commandLine\n");
    ui->shell->clear();
    ui->shell->insertPlainText(init);
    ui->shell->insertPlainText("> ");
}

MainWindow::~MainWindow()
{
    fclose(fp);
    delete ui;
}

void MainWindow::execCommand() {
    // 在commandLine中输入命令 按enter触发该槽函数执行
    int result;
    QString command = ui->commandLine->text();
    ui->commandLine->clear();
    ui->shell->insertPlainText(command);
    ui->shell->insertPlainText("\n");

    QStringList comWordList = command.split(" ");
    int wordCount = comWordList.size();
    QString func = comWordList[0];
    if(func == "" && wordCount == 1) {
        // 空命令
        ui->shell->insertPlainText("> ");
        return;
    }
    if(func == "help" && wordCount == 1) {
        QString helpInfo("A Simple File System v0.1\n"
                         "Author: Hu Ao\n\n"
                         "clear\tclear the shell\n"
                         "exit\texit the program\n"
                         "format\tformat the disk\n"
                         "help\tshow this help information\n"
                         "ls <dirpath>\t list files under dirpath\n"
                         "mkdir <dirpath>\tcreate a new dir\n"
                         "open <filepath>\topen a file\n"
                         "rm <filepath>\tremove a file\n"
                         "rmdir <dirpath>\tremove a empty dir\n"
                         "touch <filepath>\tcreate a new file\n");
        ui->shell->insertPlainText(helpInfo);
    } else if(func == "exit" && wordCount == 1) {
        emit exitProgram();
    } else if(func == "clear" && wordCount == 1) {
        ui->shell->clear();
    } else if(func == "format" && wordCount == 1) {
        format(fp);
        ui->shell->insertPlainText("Format disk finished\n");
    } else if(func == "touch" && wordCount == 2) {
        QString newFilePath = comWordList[1];
        result = createFile(newFilePath.toStdString().c_str(), fp);
        if(result == 0) {
            ui->shell->insertPlainText("Create file successful\n");
        } else {
            ui->shell->insertPlainText(QString::number(result) + "\n");
        }
    } else if(func == "mkdir" && wordCount == 2) {
        QString newDirPath = comWordList[1];
        result = createDir(newDirPath.toStdString().c_str(), fp);
        if(result == 0) {
            ui->shell->insertPlainText("Create dir successful\n");
        } else {
            ui->shell->insertPlainText(QString::number(result) + "\n");
        }
    } else if(func == "open" && wordCount == 2) {
        ui->commandLine->setEnabled(false);
        ui->WriteClose->setEnabled(true);
        QString filePath = comWordList[1];
        fileNodeOffset = openFile(filePath.toStdString().c_str(), fp, ui);
        if(fileNodeOffset < 0) {
            ui->shell->insertPlainText(QString::number(fileNodeOffset) + "\n");
            ui->commandLine->setEnabled(true);
            ui->WriteClose->setEnabled(false);
        } else {
            ui->shell->setReadOnly(false);      // 将shell修改为可写模式
            return;
        }
    } else if(func == "ls" && wordCount == 2) {
        QString dirPath = comWordList[1];
        result = listFile(dirPath.toStdString().c_str(), fp, ui);
        if(result != 0) {
            ui->shell->insertPlainText(QString::number(result) + "\n");
        }
    } else if(func == "rm" && wordCount == 2) {
        QString filepath = comWordList[1];
        result = rmFile(filepath.toStdString().c_str(), fp);
        if(result == 0) {
            ui->shell->insertPlainText("remove file successful\n");
        } else {
            ui->shell->insertPlainText(QString::number(result) + "\n");
        }
    } else if(func == "rmdir" && wordCount == 2) {
        QString dirpath = comWordList[1];
        result = rmDir(dirpath.toStdString().c_str(), fp);
        if(result == 0) {
            ui->shell->insertPlainText("remove directory successful\n");
        } else {
            ui->shell->insertPlainText(QString::number(result) + "\n");
        }
    } else {
        QString errorInfo("Command Format Error, input 'help' for more information\n");
        ui->shell->insertPlainText(errorInfo);
    }

    ui->shell->insertPlainText("> ");
    return;
}

void MainWindow::on_shell_textChanged() {
    ui->shell->moveCursor(QTextCursor::End);    // 当shell窗口中的内容发生变化时，自动滚动到窗口最底部
}

int MainWindow::setFp(FILE *fp) {
    this->fp = fp;
    return 0;
}

void MainWindow::on_writeClose() {
    // 在shell中编辑文件完成后点击按钮 触发此槽函数 然后我们把shell中的内容写入到文件系统中
    ui->shell->setReadOnly(true);   // shell修改为只读
    QString fileContent = ui->shell->toPlainText();
    writeClose(fileContent.toStdString().c_str(), fileNodeOffset, fp);

    ui->shell->clear();
    ui->shell->setPlainText(originText);    // 恢复shell中原有的内容
    ui->commandLine->setEnabled(true);
    ui->WriteClose->setEnabled(false);      // 命令行可输入，写入并关闭文件按钮无效
    ui->shell->insertPlainText("> ");
    return;
}












