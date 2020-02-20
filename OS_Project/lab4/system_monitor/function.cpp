#include "function.h"

int getCPUusage() {
    long int user, nice, sys, idle, iowait, irq, softirq;
    long int all1, all2, idle1, idle2;
    char tempBuf[20];
    QFile stat("/proc/stat");
    stat.open(QIODevice::ReadOnly);
    QByteArray statContent = stat.readAll();
    stat.close();

    char *statPointer = statContent.data();
    sscanf(statPointer, "%s%ld%ld%ld%ld%ld%ld%ld", tempBuf, &user, &nice, &sys,
           &idle, &iowait, &irq, &softirq);
    all1 = user + nice + sys + idle + iowait + irq + softirq;
    idle1 = idle;
    stat.close();
    usleep(50000); // 0.05s
    stat.open(QIODevice::ReadOnly);
    statContent = stat.readAll();
    stat.close();
    statPointer = statContent.data();
    sscanf(statPointer, "%s%ld%ld%ld%ld%ld%ld%ld", tempBuf, &user, &nice, &sys,
           &idle, &iowait, &irq, &softirq);
    all2 = user + nice + sys + idle + iowait + irq + softirq;
    idle2 = idle;

    return 100 * (all2 - all1 - idle2 + idle1) / (all2 - all1);
}

int getPhyMemUsage() {
    int from, end;
    QString tempStr;

    QFile meminfo("/proc/meminfo");
    meminfo.open(QIODevice::ReadOnly);
    QByteArray meminfoContent = meminfo.readAll();
    meminfo.close();
    QString meminfoStr(meminfoContent);

    // Total Physical Memory
    from = meminfoStr.indexOf("MemTotal");
    end = meminfoStr.indexOf("MemFree");
    QString TotalPhyMem(meminfoStr.mid(from + 9, end - from - 13));
    unsigned int TotalPhyMemNum = TotalPhyMem.toUInt();

    // Free Physical Memory
    from = meminfoStr.indexOf("MemFree");
    end = meminfoStr.indexOf("MemAvailable");
    QString FreePhyMem(meminfoStr.mid(from + 8, end - from - 12));
    unsigned int FreePhyMemNum = FreePhyMem.toUInt();

    return (TotalPhyMemNum - FreePhyMemNum) * 100 / TotalPhyMemNum;
}

int setAuthorInfo(Ui::MainWindow *ui) {
    // 填写作者info
    ui->help_info->setText(
        "System Monitor v0.1\nAuthor: Hu Ao\nEmail: u201714761@hust.edu.cn\n");
    return 0;
}

int setCPUinfo(Ui::MainWindow *ui) {
    int from, end;
    QFile cpuinfo("/proc/cpuinfo");
    cpuinfo.open(QIODevice::ReadOnly);
    QByteArray cpuinfoContent = cpuinfo.readAll();
    cpuinfo.close();

    QString cpuinfoStr(cpuinfoContent);
    // find vendor
    from = cpuinfoStr.indexOf("vendor_id");
    end = cpuinfoStr.indexOf("cpu family");
    QString cpuType(cpuinfoStr.mid(from + 12, end - from - 13));
    ui->CPUType->setText(cpuType);
    // find cpu name
    from = cpuinfoStr.indexOf("model name");
    end = cpuinfoStr.indexOf("stepping");
    QString cpuName(cpuinfoStr.mid(from + 13, end - from - 14));
    ui->CPUName->setText(cpuName);
    // find cpu freq
    from = cpuinfoStr.indexOf("cpu MHz");
    end = cpuinfoStr.indexOf("cache size");
    QString cpuFreq(cpuinfoStr.mid(from + 11, end - from - 12));
    ui->CPUFreq->setText(cpuFreq + "MHz");
    // find cache size
    from = cpuinfoStr.indexOf("cache size");
    end = cpuinfoStr.indexOf("physical id");
    QString cacheSize(cpuinfoStr.mid(from + 13, end - from - 14));
    ui->CacheSize->setText(cacheSize);
    // find logical core number
    from = cpuinfoStr.indexOf("siblings");
    end = cpuinfoStr.indexOf("core id");
    QString cpuSiblingsNum(cpuinfoStr.mid(from + 11, end - from - 12));
    ui->CPUSiblingsNum->setText(cpuSiblingsNum);
    // find physical core number
    from = cpuinfoStr.indexOf("cpu cores");
    end = cpuinfoStr.indexOf("apicid");
    QString cpuCoreNum(cpuinfoStr.mid(from + 12, end - from - 13));
    ui->CPUCoreNum->setText(cpuCoreNum);

    return 0;
}

int setOSinfo(Ui::MainWindow *ui) {
    int from, end;
    QFile version("/proc/version");
    version.open(QIODevice::ReadOnly);
    QByteArray OSinfoContent = version.readAll();
    version.close();

    QString OSinfo(OSinfoContent);
    // find GCC version
    from = OSinfo.indexOf("gcc");
    end = OSinfo.indexOf("#");
    QString GCCversion(OSinfo.mid(from, end - from - 2));
    ui->GCCVersion->setText(GCCversion);

    // OS type
    QFile type("/proc/sys/kernel/ostype");
    type.open(QIODevice::ReadOnly);
    QByteArray typeContent = type.readAll();
    type.close();
    typeContent[typeContent.size() - 1] = 0;
    QString typeStr(typeContent);
    ui->OSType->setText(typeStr);

    // OS release
    QFile release("/proc/sys/kernel/osrelease");
    release.open(QIODevice::ReadOnly);
    QByteArray releaseContent = release.readAll();
    release.close();
    releaseContent[releaseContent.size() - 1] = 0;
    QString releaseStr(releaseContent);
    ui->OSRelease->setText(releaseStr);

    // OS hostname(set when build linux kernel)
    QFile hostname("/proc/sys/kernel/hostname");
    hostname.open(QIODevice::ReadOnly);
    QByteArray hostnameContent = hostname.readAll();
    hostname.close();
    hostnameContent[hostnameContent.size() - 1] = 0;
    QString hostnameStr(hostnameContent);
    ui->OSHostname->setText(hostnameStr);

    return 0;
}

int setMemory(Ui::MainWindow *ui) {
    int from, end;
    QString tempStr;

    QFile meminfo("/proc/meminfo");
    meminfo.open(QIODevice::ReadOnly);
    QByteArray meminfoContent = meminfo.readAll();
    meminfo.close();
    QString meminfoStr(meminfoContent);
    // Total Physical Memory
    from = meminfoStr.indexOf("MemTotal");
    end = meminfoStr.indexOf("MemFree");
    QString TotalPhyMem(meminfoStr.mid(from + 9, end - from - 13));
    ui->TotalPhyMem->setText(TotalPhyMem + "KiB=");
    unsigned int TotalPhyMemNum = TotalPhyMem.toUInt();
    if ((TotalPhyMemNum >> 10) != 0) {
        if ((TotalPhyMemNum >> 20) != 0) {
            tempStr = QString::number(TotalPhyMemNum / 1048576.0, 'f', 2);
            ui->TotalPhyMemLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(TotalPhyMemNum / 1024.0, 'f', 2);
            ui->TotalPhyMemLess->setText(tempStr + "MiB");
        }
    } else {
        ui->TotalPhyMemLess->setText(TotalPhyMem + "KiB");
    }

    // Free Physical Memory
    from = meminfoStr.indexOf("MemFree");
    end = meminfoStr.indexOf("MemAvailable");
    QString FreePhyMem(meminfoStr.mid(from + 8, end - from - 12));
    ui->FreePhyMem->setText(FreePhyMem + "KiB=");
    unsigned int FreePhyMemNum = FreePhyMem.toUInt();
    if ((FreePhyMemNum >> 10) != 0) {
        if ((FreePhyMemNum >> 20) != 0) {
            tempStr = QString::number(FreePhyMemNum / 1048576.0, 'f', 2);
            ui->FreePhyMemLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(FreePhyMemNum / 1024.0, 'f', 2);
            ui->FreePhyMemLess->setText(tempStr + "MiB");
        }
    } else {
        ui->FreePhyMemLess->setText(FreePhyMem + "KiB");
    }

    // Shared Memory
    from = meminfoStr.indexOf("Shmem");
    end = meminfoStr.indexOf("KReclaimable");
    QString ShrMem(meminfoStr.mid(from + 6, end - from - 10));
    ui->ShrMem->setText(ShrMem + "KiB=");
    unsigned int ShrMemNum = ShrMem.toUInt();
    if ((ShrMemNum >> 10) != 0) {
        if ((ShrMemNum >> 20) != 0) {
            tempStr = QString::number(ShrMemNum / 1048576.0, 'f', 2);
            ui->ShrMemLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(ShrMemNum / 1024.0, 'f', 2);
            ui->ShrMemLess->setText(tempStr + "MiB");
        }
    } else {
        ui->ShrMemLess->setText(ShrMem + "KiB");
    }

    // Disk Buffers
    from = meminfoStr.indexOf("Buffers");
    end = meminfoStr.indexOf("Cached");
    QString DiskBuf(meminfoStr.mid(from + 8, end - from - 12));
    ui->DiskBuf->setText(DiskBuf + "KiB=");
    unsigned int DiskBufNum = DiskBuf.toUInt();
    if ((DiskBufNum >> 10) != 0) {
        if ((DiskBufNum >> 20) != 0) {
            tempStr = QString::number(DiskBufNum / 1048576.0, 'f', 2);
            ui->DiskBufLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(DiskBufNum / 1024.0, 'f', 2);
            ui->DiskBufLess->setText(tempStr + "MiB");
        }
    } else {
        ui->DiskBufLess->setText(DiskBuf + "KiB");
    }

    // Disk Cached
    from = meminfoStr.indexOf("Cached");
    end = meminfoStr.indexOf("SwapCached");
    QString DiskCached(meminfoStr.mid(from + 7, end - from - 11));
    ui->DiskCache->setText(DiskCached + "KiB=");
    unsigned int DiskCachedNum = DiskCached.toUInt();
    if ((DiskCachedNum >> 10) != 0) {
        if ((DiskCachedNum >> 20) != 0) {
            tempStr = QString::number(DiskCachedNum / 1048576.0, 'f', 2);
            ui->DiskCacheLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(DiskCachedNum / 1024.0, 'f', 2);
            ui->DiskCacheLess->setText(tempStr + "MiB");
        }
    } else {
        ui->DiskCacheLess->setText(DiskCached + "KiB");
    }

    // Total Swap
    from = meminfoStr.indexOf("SwapTotal");
    end = meminfoStr.indexOf("SwapFree");
    QString TotalSwap(meminfoStr.mid(from + 10, end - from - 14));
    ui->TotalSwap->setText(TotalSwap + "KiB=");
    unsigned int TotalSwapNum = TotalSwap.toUInt();
    if ((TotalSwapNum >> 10) != 0) {
        if ((TotalSwapNum >> 20) != 0) {
            tempStr = QString::number(TotalSwapNum / 1048576.0, 'f', 2);
            ui->TotalSwapLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(TotalSwapNum / 1024.0, 'f', 2);
            ui->TotalSwapLess->setText(tempStr + "MiB");
        }
    } else {
        ui->TotalSwapLess->setText(TotalSwap + "KiB");
    }

    // Free Swap
    from = meminfoStr.indexOf("SwapFree");
    end = meminfoStr.indexOf("Dirty");
    QString FreeSwap(meminfoStr.mid(from + 9, end - from - 13));
    ui->FreeSwap->setText(FreeSwap + "KiB=");
    unsigned int FreeSwapNum = FreeSwap.toUInt();
    if ((FreeSwapNum >> 10) != 0) {
        if ((FreeSwapNum >> 20) != 0) {
            tempStr = QString::number(FreeSwapNum / 1048576.0, 'f', 2);
            ui->FreeSwapLess->setText(tempStr + "GiB");
        } else {
            tempStr = QString::number(FreeSwapNum / 1024.0, 'f', 2);
            ui->FreeSwapLess->setText(tempStr + "MiB");
        }
    } else {
        ui->FreeSwapLess->setText(TotalSwap + "KiB");
    }

    ui->TotalMem->setRange(0, 100);
    ui->TotalMem->setValue(
        (TotalPhyMemNum + TotalSwapNum - FreePhyMemNum - FreeSwapNum) * 100 /
        (TotalPhyMemNum + TotalSwapNum));
    ui->PhyMem->setRange(0, 100);
    ui->PhyMem->setValue((TotalPhyMemNum - FreePhyMemNum) * 100 /
                         TotalPhyMemNum);
    ui->Swap->setRange(0, 100);
    ui->Swap->setValue((TotalSwapNum - FreeSwapNum) * 100 / TotalSwapNum);

    unsigned int UsedMemoryNum = TotalPhyMemNum - FreePhyMemNum;
    QString UsedMemory;
    if ((UsedMemoryNum >> 10) != 0) {
        if ((UsedMemoryNum >> 20) != 0) {
            UsedMemory = QString::number(UsedMemoryNum / 1048576.0, 'f', 2);
            UsedMemory = UsedMemory + "GiB";
        } else {
            UsedMemory = QString::number(UsedMemoryNum / 1024.0, 'f', 2);
            UsedMemory = UsedMemory + "MiB";
        }
    } else {
        UsedMemory = QString::number(UsedMemoryNum) + "KiB";
    }

    QString TotalMemory;
    if ((TotalPhyMemNum >> 10) != 0) {
        if ((TotalPhyMemNum >> 20) != 0) {
            TotalMemory = QString::number(TotalPhyMemNum / 1048576.0, 'f', 2);
            TotalMemory = TotalMemory + "GiB";
        } else {
            TotalMemory = QString::number(TotalPhyMemNum / 1024.0, 'f', 2);
            TotalMemory = TotalMemory + "MiB";
        }
    } else {
        TotalMemory = QString::number(TotalPhyMemNum) + "KiB";
    }

    ui->MemUsage->setText("Memory usage: " + UsedMemory + " / " + TotalMemory);

    return 0;
}

int setCPUusage(Ui::MainWindow *ui) {
    int cpuUsage = getCPUusage();
    //    ui->CPUusage->setRange(0, 100);
    //    ui->CPUusage->setValue(cpuUsage);
    ui->CPUusage_2->setText("CPU usage: " + QString::number(cpuUsage) + "%");
    return 0;
}

int initProcess(Ui::MainWindow *ui) {
    ui->ProcessTable->setRowCount(0);
    ui->ProcessTable->setColumnCount(8);
    QStringList headers;
    headers << QStringLiteral("pid") << QStringLiteral("name")
            << QStringLiteral("status") << QStringLiteral("priority")
            << QStringLiteral("Virt(Bytes)") << QStringLiteral("Virt")
            << QStringLiteral("Mem(Bytes)") << QStringLiteral("Mem");
    ui->ProcessTable->setHorizontalHeaderLabels(headers);
    ui->ProcessTable->verticalHeader()->setFixedWidth(0);
    ui->ProcessTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->ProcessTable->setSortingEnabled(true);
    return 0;
}

int setProcess(Ui::MainWindow *ui) {
    QDir procDir("/proc");
    QStringList procList = procDir.entryList();
    QByteArray procContent;
    QString tempStr;
    QTableWidgetItem *item;

    int row = 0;
    bool ok;
    int countStatus[4] = {0, 0, 0, 0};

    ui->ProcessTable->clearContents();
    ui->ProcessTable->setRowCount(0);

    ui->ProcessTable->setSortingEnabled(false);

    for (int i = 0; i < procList.size(); i++) {
        procList[i].toUInt(&ok);
        if (ok == false) {
            continue;
        }
        ui->ProcessTable->insertRow(row);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, procList[i].toUInt());
        ui->ProcessTable->setItem(row, 0, item);

        QFile proc("/proc/" + procList[i] + "/stat");
        proc.open(QIODevice::ReadOnly);

        procContent = proc.readAll();
        proc.close();
        QString procStr(procContent);

        // proc name
        tempStr = procStr.section(' ', 1, 1);
        tempStr = tempStr.mid(1, tempStr.size() - 2);
        ui->ProcessTable->setItem(row, 1, new QTableWidgetItem(tempStr));

        // proc status
        tempStr = procStr.section(' ', 2, 2);
        switch (tempStr[0].toLatin1()) {
        case 'R':
            ui->ProcessTable->setItem(row, 2, new QTableWidgetItem("Running"));
            countStatus[0]++;
            break;
        case 'S':
            ui->ProcessTable->setItem(row, 2, new QTableWidgetItem("Sleeping"));
            countStatus[1]++;
            break;
        case 'D':
            ui->ProcessTable->setItem(row, 2,
                                      new QTableWidgetItem("Disk sleep"));
            break;
        case 'T':
            ui->ProcessTable->setItem(
                row, 2, new QTableWidgetItem("Stopped(on a signal)"));
            countStatus[2]++;
            break;
        case 't':
            ui->ProcessTable->setItem(row, 2,
                                      new QTableWidgetItem("Tracing Stop"));
            countStatus[2]++;
            break;
        case 'Z':
            ui->ProcessTable->setItem(row, 2, new QTableWidgetItem("Zombie"));
            countStatus[3]++;
            break;
        case 'X':
            ui->ProcessTable->setItem(row, 2, new QTableWidgetItem("Dead"));
            break;
        default:
            ui->ProcessTable->setItem(row, 2, new QTableWidgetItem(tempStr));
            break;
        }

        // proc priority
        tempStr = procStr.section(' ', 17, 17);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, tempStr.toInt());
        ui->ProcessTable->setItem(row, 3, item);

        // virtual memory
        tempStr = procStr.section(' ', 22, 22);
        unsigned long int vMemSize = tempStr.toULong();
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, tempStr.toLongLong());
        ui->ProcessTable->setItem(row, 4, item);
        if ((vMemSize >> 10) != 0) {
            if ((vMemSize >> 20) != 0) {
                if ((vMemSize >> 30) != 0) {
                    tempStr =
                        QString::number(vMemSize / 1048576.0 / 1024.0, 'f', 2);
                    ui->ProcessTable->setItem(
                        row, 5, new QTableWidgetItem(tempStr + "GiB"));
                } else {
                    tempStr = QString::number(vMemSize / 1048576.0, 'f', 2);
                    ui->ProcessTable->setItem(
                        row, 5, new QTableWidgetItem(tempStr + "MiB"));
                }
            } else {
                tempStr = QString::number(vMemSize / 1024.0, 'f', 2);
                ui->ProcessTable->setItem(
                    row, 5, new QTableWidgetItem(tempStr + "KiB"));
            }
        } else {
            ui->ProcessTable->setItem(row, 5,
                                      new QTableWidgetItem(tempStr + "Bytes"));
        }

        // physical memory
        tempStr = procStr.section(' ', 23, 23);
        unsigned long int pMemPages = tempStr.toULong();
        qlonglong pMemSize = pMemPages * (getpagesize());
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, pMemSize);
        ui->ProcessTable->setItem(row, 6, item);
        if ((pMemSize >> 10) != 0) {
            if ((pMemSize >> 20) != 0) {
                if ((pMemSize >> 30) != 0) {
                    tempStr =
                        QString::number(pMemSize / 1048576.0 / 1024.0, 'f', 2);
                    ui->ProcessTable->setItem(
                        row, 7, new QTableWidgetItem(tempStr + "GiB"));
                } else {
                    tempStr = QString::number(pMemSize / 1048576.0, 'f', 2);
                    ui->ProcessTable->setItem(
                        row, 7, new QTableWidgetItem(tempStr + "MiB"));
                }
            } else {
                tempStr = QString::number(pMemSize / 1024.0, 'f', 2);
                ui->ProcessTable->setItem(
                    row, 7, new QTableWidgetItem(tempStr + "KiB"));
            }
        } else {
            ui->ProcessTable->setItem(row, 7,
                                      new QTableWidgetItem(tempStr + "Bytes"));
        }
        row++;
    }
    ui->AllNum->setNum(row);
    ui->RunningNum->setNum(countStatus[0]);
    ui->SleepingNum->setNum(countStatus[1]);
    ui->TracedStopNum->setNum(countStatus[2]);
    ui->ZombieNum->setNum(countStatus[3]);

    ui->ProcessTable->setSortingEnabled(true);
    return 0;
}

int initModule(Ui::MainWindow *ui) {
    QStringList headers;
    headers << QStringLiteral("Module Name") << QStringLiteral("Memory")
            << QStringLiteral("Used Times");
    ui->ModulesTable->setHorizontalHeaderLabels(headers);
    ui->ModulesTable->verticalHeader()->setFixedWidth(0);
    ui->ModulesTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->ModulesTable->setRowCount(0);
    ui->ModulesTable->setColumnCount(3);
    ui->ModulesTable->setSortingEnabled(true);
    return 0;
}

int setModule(Ui::MainWindow *ui) {
    QFile module("/proc/modules");
    module.open(QIODevice::ReadOnly);
    QByteArray moduleContent = module.readAll();
    module.close();
    QString moduleStr(moduleContent);
    QStringList moduleList = moduleStr.split("\n");
    QStringList moduleInfoList;
    QTableWidgetItem *item;

    ui->ModulesTable->clearContents();
    ui->ModulesTable->setRowCount(0);
    ui->ModulesTable->setSortingEnabled(false);

    for (int i = 0; i < moduleList.size() - 1; i++) {
        moduleInfoList = moduleList[i].split(" ");
        ui->ModulesTable->insertRow(i);
        ui->ModulesTable->setItem(i, 0,
                                  new QTableWidgetItem(moduleInfoList[0]));
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, moduleInfoList[1].toLongLong());
        ui->ModulesTable->setItem(i, 1, item);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, moduleInfoList[2].toLongLong());
        ui->ModulesTable->setItem(i, 2, item);
    }

    ui->ModulesTable->setSortingEnabled(true);
    return 0;
}

int setTime(Ui::MainWindow *ui) {
    ui->Time->setText("Time: " + QDateTime::currentDateTime().toString(
                                     "yyyy.MM.dd hh:mm:ss"));
    return 0;
}

int setBootTime(Ui::MainWindow *ui) {
    QFile uptime("/proc/uptime");
    uptime.open(QIODevice::ReadOnly);
    QByteArray uptimeContent = uptime.readAll();
    uptime.close();
    QString uptimeStr(uptimeContent);
    uptimeStr = uptimeStr.split(" ")[0];
    uptimeStr = uptimeStr.split(".")[0];
    qint64 uptimeSec = uptimeStr.toLongLong();

    QDateTime nowTime = QDateTime::currentDateTime();
    QDateTime bootTime = nowTime.addSecs(-uptimeSec);

    ui->LastBootTime->setText(bootTime.toString("yyyy.MM.dd hh:mm:ss"));
    uptimeStr = "";
    if (uptimeSec < 60) {
        uptimeStr = QString::number(uptimeSec) + "s";
    } else if (uptimeSec < 60 * 60) {
        uptimeStr = QString::number(uptimeSec / 60) + "m" +
                    QString::number(uptimeSec % 60) + "s";
    } else {
        uptimeStr = QString::number(uptimeSec / 3600) + "h" +
                    QString::number((uptimeSec % 3600) / 60) + "m" +
                    QString::number(uptimeSec % 60) + "s";
    }
    ui->UpTime->setText(uptimeStr);
    return 0;
}

int updateDraw() {
    QVector<QPointF> oldPoints = cpuSeries->pointsVector();
    QVector<QPointF> newPoints;
    for (int i = 0; i < DRAW_SIZE - 1; i++) {
        newPoints.append(QPointF(i, oldPoints.at(i + 1).y()));
    }
    newPoints.append(QPointF(DRAW_SIZE - 1, getCPUusage() / 100.0));
    cpuSeries->replace(newPoints);

    oldPoints = memSeries->pointsVector();
    newPoints.clear();
    for (int i = 0; i < DRAW_SIZE - 1; i++) {
        newPoints.append(QPointF(i, oldPoints.at(i + 1).y()));
    }
    newPoints.append(QPointF(DRAW_SIZE - 1, getPhyMemUsage() / 100.0));
    memSeries->replace(newPoints);

    return 0;
}
