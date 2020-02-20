#ifndef FUNCTION_H
#define FUNCTION_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QChartView>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLineSeries>
#include <QValueAxis>
#include <QtCharts/QtCharts>
#include <QtCharts>
#include <signal.h>
#include <unistd.h>

#define DRAW_SIZE (5 * 120)

int setAuthorInfo(Ui::MainWindow *);
int setCPUinfo(Ui::MainWindow *);
int setOSinfo(Ui::MainWindow *);
int setMemory(Ui::MainWindow *);
int setCPUusage(Ui::MainWindow *);
int initProcess(Ui::MainWindow *);
int setProcess(Ui::MainWindow *);
int initModule(Ui::MainWindow *);
int setModule(Ui::MainWindow *);
int setTime(Ui::MainWindow *);
int setBootTime(Ui::MainWindow *);
int updateDraw();

extern QLineSeries *cpuSeries;
extern QLineSeries *memSeries;

#endif // FUNCTION_H
