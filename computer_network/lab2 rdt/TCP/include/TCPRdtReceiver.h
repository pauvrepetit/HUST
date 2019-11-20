#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H
#include "RdtReceiver.h"
class TCPRdtReceiver : public RdtReceiver {
  private:
  public:
    int expectSeqNum;
    Packet ackPkg;

    TCPRdtReceiver();
    // virtual ~GBNRdtReceiver();

  public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
};

#endif
