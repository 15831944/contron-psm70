#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "sys/platform.h"

class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject();
protected:
    void enter();
    void leave();
private:
    CRITICAL_SECTION mCritical;
};

#endif // BASEOBJECT_H
