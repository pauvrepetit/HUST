#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "RdtSender.h"
#include "Global.h"
class GBNRdtSender :public RdtSender
{
private:

public:
    int seqNum;
    int windowBase;
    int countPackage;
    Packet sendPackage[SEQ_MAX + 1];

	bool getWaitingState();
	bool send(const Message &message);
	void receive(const Packet &ackPkt);
	void timeoutHandler(int seqNum);

public:
	GBNRdtSender();
	// virtual ~GBNRdtSender();
};

#endif
