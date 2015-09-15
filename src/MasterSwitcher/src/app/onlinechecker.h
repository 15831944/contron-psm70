#ifndef ONLINECHECKER_H
#define ONLINECHECKER_H

#include "baseobject.h"
#include "ionlinechecker.h"

class OnlineChecker : public BaseObject
{
public:
    OnlineChecker();
    ~OnlineChecker();

    void setOnlineChecker(IOnlineChecker *checker);

    void setCheckInterval(int interval);
    int getCheckInterval() { return mCheckInterval; }

    void exec();

public:
    virtual void checkOnline();

private:
    IOnlineChecker *mChecker;
    int mCheckInterval;

    pthread_t mThread[1];
};

#endif // ONLINECHECKER_H
