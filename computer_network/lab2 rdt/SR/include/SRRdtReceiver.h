#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "Global.h"
#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver {
  public:
    int windowBase;
    Packet receivePacket[SEQ_MAX + 1];
    Packet ackPacket;

  public:
    SRRdtReceiver();

  public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
};

#endif