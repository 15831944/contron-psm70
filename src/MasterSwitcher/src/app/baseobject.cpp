#include "baseobject.h"

BaseObject::BaseObject()
{
    MUTEX_INIT(&mCritical);
}

BaseObject::~BaseObject()
{
    MUTEX_FREE(&mCritical);
}

void BaseObject::enter()
{
    MUTEX_LOCK(&mCritical);
}

void BaseObject::leave()
{
    MUTEX_UNLOCK(&mCritical);
}

