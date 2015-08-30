#ifndef ONLINECHECKER_H
#define ONLINECHECKER_H

#include "baseobject.h"
#include "ionlinechecker.h"

class OnlineChecker : public BaseObject
{
public:
    OnlineChecker();
    ~OnlineChecker();

    void exec();
    void checkOnline();

    void setOnlineChecker(IOnlineChecker *checker);

    void setCheckInterval(int interval);
    int getCheckInterval() { return mCheckInterval; }

private:
    IOnlineChecker *mChecker;
    int mCheckInterval;

    pthread_t mThread[1];
};

#endif // ONLINECHECKER_H
