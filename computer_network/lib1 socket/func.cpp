#include <QString>
#include <QTextStream>
#include "mysocket.h"
#include <iostream>

// QString IP addr to unsigned int IP addr
unsigned int IPtoINT(QString IPAddr) {
    QTextStream streamIn(&IPAddr);
    unsigned int IPNum = 0;
    unsigned int IP1, IP2, IP3, IP4;
    char temp;
    streamIn >> IP1 >> temp >> IP2 >> temp >> IP3 >> temp >> IP4;
    IPNum = (IP1 << 24) + (IP2 << 16) + (IP3 << 8) + IP4;
    return IPNum;
}

void closeSocket(int socket) {
    close(socket);
}

QString INTtoIP(unsigned int IPAddr){
    IPAddr = ntohl(IPAddr);
    QString IPstr = QString::number(IPAddr >> 24) + "." + QString::number((IPAddr >> 16) & 0xFF) + "." + QString::number((IPAddr >> 8) & 0xFF) + "." + QString::number(IPAddr & 0xFF);
    return IPstr;
}
