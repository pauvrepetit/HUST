#include "TCPRdtReceiver.h"
#include "Global.h"

TCPRdtReceiver::TCPRdtReceiver() {
    expectSeqNum = 0;
    ackPkg.acknum = -1;
    //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    ackPkg.checksum = 0;
    ackPkg.seqnum = -1; //忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
        ackPkg.payload[i] = '.';
    }
    ackPkg.checksum = pUtils->calculateCheckSum(ackPkg);
}

void TCPRdtReceiver::receive(const Packet &packet) {
    int checkSum = pUtils->calculateCheckSum(packet); // 计算检查和

    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && expectSeqNum == packet.seqnum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);

        //取出Message，向上递交给应用层
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);

        ackPkg.acknum = packet.acknum;
        ackPkg.seqnum = packet.seqnum;
        ackPkg.checksum = pUtils->calculateCheckSum(ackPkg);
        pUtils->printPacket("接收方发送确认报文", ackPkg);
        pns->sendToNetworkLayer(SENDER, ackPkg);

        expectSeqNum = (expectSeqNum + 1) % (SEQ_MAX + 1);
    } else {
        if (checkSum != packet.checksum) {
            pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误",
                                packet);
        } else {
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对",
                                packet);
        }
        pUtils->printPacket("接收方重新发送上次的确认报文", ackPkg);
        pns->sendToNetworkLayer(SENDER, ackPkg);
        //调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
    }
}