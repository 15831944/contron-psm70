#ifndef NETWORKSTATE_H
#define NETWORKSTATE_H

enum NETWORK_STATE {
    NETWORK_ONLINE,
    NETWORK_OFFLINE
};

class NetworkState
{
public:
    NetworkState();
    ~NetworkState();

    void setIp(char *ip);
    NETWORK_STATE getState();
    void *getOwner();
    void setOwner(void *owner);
private:
    char *mIp;
    void *mOwner;
    NETWORK_STATE mState;
};

#endif // NETWORKSTATE_H
