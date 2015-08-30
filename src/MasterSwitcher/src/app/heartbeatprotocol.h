#ifndef HEARTBEATPROTOCOL_H
#define HEARTBEATPROTOCOL_H

#include "heartbeat.h"

class HeartbeatProtocol
{
public:
    HeartbeatProtocol();
    ~HeartbeatProtocol();

    Heartbeat *find(char *buffer, int size);
    Heartbeat *makeHeartbeat(bool isSlave, double timePoint);
};

#endif // HEARTBEATPROTOCOL_H
