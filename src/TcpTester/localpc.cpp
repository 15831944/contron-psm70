#include "localpc.h"

#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "sys/platform.h"
#include "utils.h"

#if QT
#include "log.h"
#define DEBUG_OUTPUT APP_LOG
#else
#define DEBUG_OUTPUT printf
#endif


LocalPC::LocalPC()
    : BaseObject()
    , ITcpServer()
{
    mTcpServer = new TcpServer();
    mTcpServer->setHandler(this);
}
LocalPC::~LocalPC()
{
    enter();
    mTcpServer->close();
    leave();

    delete mTcpServer;
}

void LocalPC::setPort(int port)
{
    enter();
    mTcpServer->setPort(port);
    leave();
}

int LocalPC::start()
{
    int result = NUMBER_FALSE;

    enter();
    result = mTcpServer->start();
    leave();

    return result;
}

void LocalPC::tcpServerReceiveData(void *tcp, char *buffer, int size)
{
    enter();

    printf("[LocalPC]receive:\n%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        delete hb;

        char *p = NULL;
        int size = 0;
        bool isSlave = false;
        double timePoint;
        GET_TIME(timePoint);
        Heartbeat *t = protocol.makeHeartbeat(isSlave, timePoint);
        if(NULL!=t)
        {
            if(t->makeBuffer(&p, size))
            {
                TcpClient *client = (TcpClient *)tcp;
                client->send(p, size);
                delete p;
            }
        }

    }
    leave();
}

