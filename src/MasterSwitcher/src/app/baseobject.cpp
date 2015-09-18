#include "baseobject.h"

BaseObject::BaseObject()
{
    MUTEX_INIT(&mCritical);
    mExiting = false;
}

BaseObject::~BaseObject()
{
    MUTEX_FREE(&mCritical);
}

bool BaseObject::isExiting()
{
    bool result = false;
    enter();
    result = mExiting;
    leave();
    return result;
}

void BaseObject::setExiting(bool exiting)
{
    enter();
    mExiting = exiting;
    leave();
}

void BaseObject::enter()
{
    MUTEX_LOCK(&mCritical);
}

void BaseObject::leave()
{
    MUTEX_UNLOCK(&mCritical);
}

