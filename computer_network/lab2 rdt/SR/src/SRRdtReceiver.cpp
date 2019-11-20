#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver() {
    windowBase = 0;
    ackPacket.seqnum = -1;
    ackPacket.acknum = -1;
    ackPacket.checksum = 0;
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
        ackPacket.payload[i] = '.';
    }
    ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
    for (int i = 0; i < SEQ_MAX; i++) {
        receivePacket[i].acknum = 0;
        // 初值赋为0,用于标记是否是缓存数据
    }
}

void SRRdtReceiver::receive(const Packet &packet) {
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum) {
        // 检查和正确
        pUtils->printPacket("接收方正确收到发送方的报文", packet);

        // 首先发送相应的ack
        ackPacket.seqnum = packet.seqnum;
        ackPacket.checksum = 0;
        ackPacket.acknum = -1;
        ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
        pUtils->printPacket("接收方发送ACK报文", ackPacket);
        pns->sendToNetworkLayer(SENDER, ackPacket);
        // 然后根据是否在窗口中进行相应的操作
        if ((packet.seqnum - windowBase + SEQ_MAX + 1) % (SEQ_MAX + 1) <
            WINDOW_LENGTH) {
            // 在窗口内 先缓存
            receivePacket[packet.seqnum].seqnum = packet.seqnum;
            receivePacket[packet.seqnum].acknum = packet.acknum;
            receivePacket[packet.seqnum].checksum = packet.checksum;
            memcpy(receivePacket[packet.seqnum].payload, packet.payload,
                   sizeof(packet.payload));
            // 下面检查能否提交
            int packetSeqnum = packet.seqnum;
            if (packetSeqnum == windowBase) {
                Message msg;
                // 能提交，向上层提交数据，直到第一个acknum为0,即表明该位置不是缓存的数据
                while (receivePacket[packetSeqnum].acknum == -1) {
                    memcpy(msg.data, receivePacket[packetSeqnum].payload,
                           sizeof(receivePacket[packetSeqnum].payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    receivePacket[packetSeqnum].acknum = 0;
                    // 提交后将ack的值赋回0,表明该位置不是缓存
                    packetSeqnum++;
                    packetSeqnum %= (SEQ_MAX + 1);
                    windowBase++;
                    windowBase %= (SEQ_MAX + 1);
                }
            }
            cout << RED << "接受方向上层提交数据 当前窗口开始位置为 "
                 << windowBase << "当前窗口中数据缓存情况为";
            for (int i = 0; i < WINDOW_LENGTH; i++) {
                cout << " ";
                cout << -receivePacket[(i + windowBase) % (SEQ_MAX + 1)].acknum;
            }
            cout << endl << RESET;
        }
    } else {
        pUtils->printPacket("接收方接受报文校验错误", packet);
    }
}