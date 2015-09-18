#include "onlinechecker.h"

OnlineChecker::OnlineChecker()
    :BaseObject()
{
    mExiting = false;
}

OnlineChecker::~OnlineChecker()
{
    enter();
    if(!mExiting)
    {
        mExiting = true;
    }
    leave();

    Sleep(250);
}


THREAD_API gateway_thread(void *param)
{
    OnlineChecker *checker = (OnlineChecker *)(param);
    int now;
    GET_TIME(now);
    int interval = checker->getCheckInterval();
    int last = now;
    int idle = 200;
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        if(checker->isExiting())
        {
            break;
        }

        int current;
        GET_TIME(current);
        int timeout = last + interval;
        bool outOfTime = (current >= (timeout));
        if(outOfTime)
        {
            last = current;
            checker->checkOnline();
        }
        Sleep(idle);
    }
    return NULL;
}

void OnlineChecker::exec()
{
    int ret;
    THREAD_CREATE(&mThread[0], gateway_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(mThread[0], false);
    }
}

bool OnlineChecker::isExiting()
{
    bool result = false;
    enter();
    result = mExiting;
    leave();
    return result;
}

void OnlineChecker::checkOnline()
{
    if(NULL==mChecker)
    {
        return;
    }
    mChecker->checkOnline();
}

void OnlineChecker::setOnlineChecker(IOnlineChecker *checker)
{
    mChecker = checker;
}

void OnlineChecker::setCheckInterval(int interval)
{
    mCheckInterval = interval;
}

