#ifndef HEARTBEATPROTOCOL_H
#define HEARTBEATPROTOCOL_H

#include "heartbeat.h"

class HeartbeatProtocol
{
public:
    HeartbeatProtocol();
    ~HeartbeatProtocol();

    Heartbeat *find(char *buffer, int size);
    Heartbeat *make(UINT32 timePoint);

protected:
    Heartbeat *makeHeartbeat(bool isSlave, UINT32 timePoint);
};

#endif // HEARTBEATPROTOCOL_H
