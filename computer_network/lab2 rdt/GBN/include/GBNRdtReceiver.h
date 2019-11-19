#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver {
  private:
  public:
    int expectSeqNum;
    Packet ackPkg;

    GBNRdtReceiver();
    // virtual ~GBNRdtReceiver();

  public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
};

#endif
