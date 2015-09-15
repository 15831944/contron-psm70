#ifndef SYNC_H
#define SYNC_H

#include "onlinechecker.h"

class Sync : public OnlineChecker
{
public:
    Sync();
    ~Sync();

public:
    void checkOnline();
    void enableSync();
    void disableSync();

protected:
    void execStartSyncScript();
    void execSyncScript();
    void execStopSyncScript();

private:
    bool mEnable;
};

#endif // SYNC_H
