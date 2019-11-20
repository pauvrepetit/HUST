#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "Global.h"
#include "RdtSender.h"
class TCPRdtSender : public RdtSender {
  public:
    int seqNum;
    int windowBase;
    int countPackage;
    Packet sendPackage[SEQ_MAX + 1];
    int countACK;   // 重复ack的数量
    int ackSeq;

    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet &ackPkt);
    void timeoutHandler(int seqNum);

  public:
    TCPRdtSender();
    // virtual ~GBNRdtSender();
};

#endif
