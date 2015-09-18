#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "sys/platform.h"

#include <vector>
using namespace std;

class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject();

public:
    bool isExiting();
    virtual void setExiting(bool exiting);
protected:
    void enter();
    void leave();
private:
    CRITICAL_SECTION mCritical;
    bool mExiting;
    static vector<BaseObject *> gList;
};

#endif // BASEOBJECT_H
