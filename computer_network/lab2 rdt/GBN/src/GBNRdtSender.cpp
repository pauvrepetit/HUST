#include "GBNRdtSender.h"
#include "Global.h"
#include <cstring>

#define RESET "\033[0m"
#define RED   "\033[31m" /* Red */

GBNRdtSender::GBNRdtSender() {
    seqNum = 0;
    countPackage = 0;
    windowBase = 0;
}

bool GBNRdtSender::getWaitingState() {
    if (countPackage == WINDOW_LENGTH) {
        return true;
    }
    return false;
}

bool GBNRdtSender::send(const Message &message) {
    if (getWaitingState() == true) {
        // 窗口满
        return false;
    }

    sendPackage[seqNum].seqnum = seqNum;
    sendPackage[seqNum].acknum = -1; // 发送报文中ack信息无效
    sendPackage[seqNum].checksum = 0;
    // 参考其父类中send方法的实现，先将checksum的值置零
    memcpy(sendPackage[seqNum].payload, message.data,
           sizeof(message.data)); // payload的内容为消息主体
    sendPackage[seqNum].checksum =
        pUtils->calculateCheckSum(sendPackage[seqNum]); // 计算检查和
    // 打包完成

    pUtils->printPacket("发送方发送报文", sendPackage[seqNum]);
    // 打印相关信息
    pns->sendToNetworkLayer(RECEIVER, sendPackage[seqNum]);
    // 发送包
    if (seqNum == windowBase) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
        // 窗口的第一个包 启动定时器
    }
    seqNum = (seqNum + 1) % (SEQ_MAX + 1);
    // seqNum增加，若超过SEQ_MAX，就取模
    countPackage = (countPackage + 1) % (SEQ_MAX + 1);
    cout << RED << "当前窗口开始位置为 " << windowBase
         << ", 当前窗口中包的数量为 " << countPackage
         << endl << RESET; // 有包发出，输出当前窗口信息
    return true;
}

void GBNRdtSender::receive(const Packet &ackPkt) {
    if (countPackage != 0) {
        // 此时 是有包发出去了 但是还没有收到ACK 执行下述操作
        int checkSum = pUtils->calculateCheckSum(ackPkt); // 计算检查和
        // int ackSeqNum = ackPkt.seqnum;
        if (checkSum == ackPkt.checksum) {
            // 检查和正确
            pUtils->printPacket("发送方正确收到确认", ackPkt);

            windowBase = (ackPkt.seqnum + 1) % (SEQ_MAX + 1);
            countPackage = (seqNum - windowBase) % (SEQ_MAX + 1);
            if (countPackage < 0) {
                countPackage += (SEQ_MAX + 1);
            }
            cout << RED << "当前窗口开始位置为 " << windowBase
                 << "， 当前窗口中包的数量为 " << countPackage << endl << RESET;
            if (windowBase == seqNum) {
                // 所有的包都确认了
                pns->stopTimer(SENDER, 0);
            } else {
                // 还有包没有被确认 重启计时器
                pns->stopTimer(SENDER, 0);
                pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
            }
        } else {
            pUtils->printPacket("发送方没有正确收到确认", ackPkt);
            cout << RED << "当前窗口开始位置为 " << windowBase
                 << "， 当前窗口中包的数量为 " << countPackage << endl << RESET;
        }
    }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    pUtils->printPacket("发送方定时器时间到，重发窗口中的所有包",
                        sendPackage[windowBase]);
    cout << RED << "当前窗口开始位置为 " << windowBase
         << "， 当前窗口中包的数量为 " << countPackage << endl << RESET;
    pns->stopTimer(SENDER, 0);
    pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
    for (int i = 0; i < countPackage; i++) {
        pUtils->printPacket("重发窗口中的包", sendPackage[(i + windowBase) % (SEQ_MAX + 1)]);
        pns->sendToNetworkLayer(RECEIVER,
                                sendPackage[(i + windowBase) % (SEQ_MAX + 1)]);
        // 发送窗口中的包
    }
}