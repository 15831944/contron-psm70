#include "baseobject.h"

BaseObject::BaseObject()
    : IMutex()
{
    MUTEX_INIT(&mCritical);
    mExiting = false;
    mUser = NULL;
}

BaseObject::~BaseObject()
{
    MUTEX_FREE(&mCritical);
}

void BaseObject::setUserLock(IMutex *handler)
{
    mUser = handler;
}

void BaseObject::userLock()
{
//    if(NULL!=mUser)
//    {
//        mUser->enter();
//    }
}

void BaseObject::userLeave()
{
//    if(NULL!=mUser)
//    {
//        mUser->leave();
//    }
}

bool BaseObject::isExiting()
{
    bool result = false;
//    enter();
    result = mExiting;
//    leave();
    return result;
}

void BaseObject::setExiting(bool exiting)
{
//    enter();
    mExiting = exiting;
//    leave();
}

void BaseObject::enter()
{
    MUTEX_LOCK(&mCritical);
}

void BaseObject::leave()
{
    MUTEX_UNLOCK(&mCritical);
}

