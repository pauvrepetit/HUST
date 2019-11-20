#include "SRRdtSender.h"
#include "Global.h"

SRRdtSender::SRRdtSender() {
    seqnum = 0;
    windowBase = 0;
    countPacket = 0;
}

// 获取窗口是否满的状态
bool SRRdtSender::getWaitingState() {
    if (countPacket == WINDOW_LENGTH) {
        return true;
    }
    return false;
}

bool SRRdtSender::send(const Message &message) {
    if (getWaitingState() == true) {
        // 窗口满
        return false;
    }

    // 窗口不满 可以发送消息
    sendPacket[seqnum].seqnum = seqnum; // 序号
    sendPacket[seqnum].acknum = -1;
    sendPacket[seqnum].checksum = 0; // 检查和赋初值为0
    memcpy(sendPacket[seqnum].payload, message.data, sizeof(message.data));
    // 消息主体
    sendPacket[seqnum].checksum = pUtils->calculateCheckSum(sendPacket[seqnum]);
    // 计算检查和

    // 打包完成

    pUtils->printPacket("发送方发送报文", sendPacket[seqnum]);

    pns->sendToNetworkLayer(RECEIVER, sendPacket[seqnum]);
    // 发送报文

    pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);
    // 为每一个发出去的包设置计时器

    seqnum = (seqnum + 1) % (SEQ_MAX + 1);
    // seqnum增加，若超过SEQ_MAX，就取模
    countPacket = (countPacket + 1) % (SEQ_MAX + 1);
    cout << RED << "发送方 当前窗口开始位置为 " << windowBase
         << ", 当前窗口中包的数量为 " << countPacket << endl
         << RESET;
    // 有包发出，输出当前窗口信息
    return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
    if (countPacket != 0) {
        // 有包发出且为收到相应的响应报文
        int checkSum = pUtils->calculateCheckSum(ackPkt);
        // 计算检查和
        if (checkSum == ackPkt.checksum) {
            // 检查通过
            pUtils->printPacket("发送方收到正确的ACK报文", ackPkt);
            int ackSeqnum = ackPkt.seqnum;
            if ((ackSeqnum - windowBase + SEQ_MAX + 1) % (SEQ_MAX + 1) <=
                countPacket) {
                // 收到的ACK是窗口内的报文的ACK
                sendPacket[ackSeqnum].acknum = 0;
                // 记录包是否收到ACK，初始时，acknum为-1,置为0表明收到该包的ACK
                pns->stopTimer(SENDER, ackSeqnum);
                // 收到该包的ACK，计时器停止
                if (ackSeqnum == windowBase) {
                    // 收到的包是窗口中的第一个包，将窗口向后移动第一个ACK仍为-1的位置
                    // 或者到countPacket为0,即窗口中没有包
                    while (countPacket > 0) {
                        if (sendPacket[ackSeqnum].acknum == 0) {
                            ackSeqnum++;
                            ackSeqnum %= (SEQ_MAX + 1);
                            countPacket--;
                            windowBase++;
                            windowBase %= (SEQ_MAX + 1);
                            continue;
                        } else {
                            break;
                        }
                    }
                }
            }
        } else {
            // ACK报文出错
            pUtils->printPacket("发送方没有正确收到确认", ackPkt);
            cout << RED << "发送方 当前窗口开始位置为 " << windowBase
                 << "， 当前窗口中包的数量为 " << countPacket << endl
                 << RESET;
        }
    }
}

void SRRdtSender::timeoutHandler(int seqNum) {
    // 序号为seqNum的包超时
    pns->stopTimer(SENDER, seqNum);
    cout << "定时器" << seqNum << "超时" << endl;
    pUtils->printPacket("发送方定时器超时，重发该报文", sendPacket[seqNum]);
    pns->sendToNetworkLayer(RECEIVER, sendPacket[seqNum]);
    // 重发超时的包
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    // 重启计时器
}