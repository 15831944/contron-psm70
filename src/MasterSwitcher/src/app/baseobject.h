#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "sys/platform.h"

#include "imutex.h"

#include <vector>
using namespace std;

class BaseObject : public IMutex
{
public:
    BaseObject();
    virtual ~BaseObject();

public:
    void setUserLock(IMutex *handler);
    void userLock();
    void userLeave();

public:
    void enter();
    void leave();

public:
    bool isExiting();
    virtual void setExiting(bool exiting);

private:
    CRITICAL_SECTION mCritical;
    bool mExiting;
    IMutex *mUser;
};

#endif // BASEOBJECT_H
