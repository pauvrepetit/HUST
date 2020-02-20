#include "function.h"
#include <QMessageBox>
#include <QTimer>
#include <stdio.h>

QLineSeries *cpuSeries;
QLineSeries *memSeries;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->newProc, SIGNAL(returnPressed()), this,
            SLOT(on_newProcStart_clicked()));
    connect(ui->LoadModuleName, SIGNAL(returnPressed()), this,
            SLOT(on_LoadModule_clicked()));

    // Boot Time
    setBootTime(ui);

    // 填写状态栏
    setTime(ui);

    // 填写作者info
    setAuthorInfo(ui);

    // 填写CPU_info & OS_info
    setCPUinfo(ui);
    setOSinfo(ui);

    // About Memory
    setMemory(ui);

    // CPU usage
    setCPUusage(ui);

    // process_info
    initProcess(ui);
    setProcess(ui);

    // modules_info
    initModule(ui);
    setModule(ui);

    // CPU usage 曲线
    QChart *cpuChart = new QChart;
    QChartView *cpuView = new QChartView(cpuChart);
    cpuView->setRubberBand(QChartView::RectangleRubberBand);
    cpuView->chart()->legend()->hide();
    cpuView->setEnabled(false);
    cpuSeries = new QLineSeries;
    cpuChart->addSeries(cpuSeries);

    for (int i = 0; i < DRAW_SIZE; i++) {
        cpuSeries->append(i, 0);
    }

    cpuSeries->setUseOpenGL(true);

    QValueAxis *Xline = new QValueAxis;
    Xline->setRange(0, DRAW_SIZE);
    Xline->hide();
    Xline->setTitleText("Time");

    QValueAxis *Yline = new QValueAxis;
    Yline->setRange(0, 1);
    Yline->setTitleText("Usage");

    cpuChart->addAxis(Xline, Qt::AlignBottom);
    cpuChart->addAxis(Yline, Qt::AlignLeft);
    cpuSeries->attachAxis(Xline);
    cpuSeries->attachAxis(Yline);
    cpuChart->setTitle("CPU Usage");

    ui->CPUusageDraw->addWidget(cpuView);

    // memory usage 曲线
    QChart *memChart = new QChart;
    QChartView *memView = new QChartView(memChart);
    memView->setRubberBand(QChartView::RectangleRubberBand);
    memView->chart()->legend()->hide();
    memView->setEnabled(false);
    memSeries = new QLineSeries;
    memChart->addSeries(memSeries);

    for (int i = 0; i < DRAW_SIZE; i++) {
        memSeries->append(i, 0);
    }

    memSeries->setUseOpenGL(true);

    QValueAxis *memXline = new QValueAxis;
    memXline->setRange(0, DRAW_SIZE);
    memXline->setTitleText("Time");
    memXline->hide();

    QValueAxis *memYline = new QValueAxis;
    memYline->setRange(0, 1);
    memYline->setTitleText("Usage");

    memChart->addAxis(memXline, Qt::AlignBottom);
    memChart->addAxis(memYline, Qt::AlignLeft);
    memSeries->attachAxis(memXline);
    memSeries->attachAxis(memYline);
    memChart->setTitle("Memory Usage");

    ui->MemUsageDraw->addWidget(memView);

    // 定时更新
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
    timer->start(200);

    QTimer *second = new QTimer(this);
    connect(second, SIGNAL(timeout()), this, SLOT(updateTime()));
    second->start(1000);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_reboot_clicked() {
    // reboot
    system("reboot");
}

void MainWindow::on_shutdown_clicked() {
    // shutdown
    system("shutdown -h now");
}

void MainWindow::on_exit_clicked() {
    // exit
    emit exit_program();
}

void MainWindow::updateTime() {
    setBootTime(ui);
    setTime(ui);
}

void MainWindow::updateWindow() {
    setMemory(ui);
    setCPUusage(ui);
    updateDraw();
}

void MainWindow::on_RefreshProc_clicked() { setProcess(ui); }

void MainWindow::on_KillProc_clicked() {
    int r = ui->ProcessTable->currentRow();
    QTableWidgetItem *item = ui->ProcessTable->item(r, 0);
    QString itemText = item->text();
    kill(itemText.toUInt(), SIGINT);
    setProcess(ui);
}

void MainWindow::on_LoadModule_clicked() {
    std::string newModuleFile = ui->LoadModuleName->text().toStdString();
    std::string CommandLine = "pkexec insmod " + newModuleFile;
    system(CommandLine.c_str());
    setModule(ui);
}

void MainWindow::on_UnloadModule_clicked() {
    int r = ui->ModulesTable->currentRow();
    QTableWidgetItem *item = ui->ModulesTable->item(r, 0);
    if (item == NULL) {
        return;
    }
    QString itemText = item->text();
    QString commandLine = "pkexec rmmod " + itemText;

    system(commandLine.toStdString().c_str());

    //    QMessageBox msgBox;

    //    msgBox.exec();
    setModule(ui);
}

void MainWindow::on_newProcStart_clicked() {
    const char *commandLine = ui->newProc->text().toStdString().c_str();
    system(commandLine);
}

void MainWindow::on_RefreshModule_clicked() { setModule(ui); }
