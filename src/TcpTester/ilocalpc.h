#ifndef ILOCALPC_H
#define ILOCALPC_H

class ILocalPC
{
public:
    ILocalPC();
    ~ILocalPC();

    virtual void onLocalIsSlave()=0;
    virtual void onLocalIsMaster()=0;
};

#endif // ILOCALPC_H
