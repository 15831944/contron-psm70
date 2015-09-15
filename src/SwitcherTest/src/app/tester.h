#ifndef TESTER_H
#define TESTER_H

#include "baseobject.h"

class Tester : public BaseObject
{
public:
    Tester();
    ~Tester();

    virtual int start()=0;
};

#endif // TESTER_H
