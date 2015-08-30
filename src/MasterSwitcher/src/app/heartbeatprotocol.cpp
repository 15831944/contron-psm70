#include "heartbeatprotocol.h"

#include "sys/platform.h"

HeartbeatProtocol::HeartbeatProtocol()
{

}

HeartbeatProtocol::~HeartbeatProtocol()
{

}

Heartbeat *HeartbeatProtocol::find(char *buffer, int size)
{
    Heartbeat *hb = new Heartbeat();

    bool isValid = hb->makeFrame(buffer, size);
    if(!isValid)
    {
        delete hb;
        hb = NULL;
    }

    return hb;
}

Heartbeat *HeartbeatProtocol::makeHeartbeat(bool isSlave, double timePoint)
{
    Heartbeat *hb = new Heartbeat();

    if(!hb->setData(isSlave, timePoint))
    {
        delete hb;
        hb = NULL;
    }

    return hb;
}

