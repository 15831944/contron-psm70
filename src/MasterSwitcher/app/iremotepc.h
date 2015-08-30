#ifndef IREMOTEPC_H
#define IREMOTEPC_H


class IRemotePC
{
public:
    IRemotePC();
    ~IRemotePC();

    virtual void canBeMaster()=0;
};

#endif // IREMOTEPC_H
