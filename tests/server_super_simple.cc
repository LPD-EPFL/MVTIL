#include <iostream>

#include "ProtocolScheduler.h"

int main() {
    ProtocolScheduler * ps = new ProtocolScheduler();
    
    Key k1 = "a";
    Key k2 = "b";

    TimestampInterval t1 = {1, 5};
    TimestampInterval t2 = {2, 6};
    TimestampInterval t3 = {4, 6};
    TimestampInterval t4 = {7, 8};

    ClientReply* reply;

    reply = ps->handleRead(1, t1, k1);
    reply = ps->handleWrite(1, t2, k1, "v1");
    reply = ps->handleRead(1, t3, k2);
    reply = ps->handleHintRequest(t4, k2);

    reply = ps->handleCommit(1, 4);

    return 0;
}
