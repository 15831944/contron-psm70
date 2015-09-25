#include "gateway.h"

#include "sys/platform.h"

#include "ping.h"

Gateway::Gateway()
    :BaseObject(), IOnlineChecker()
{
    mState = GATEWAY_ONLINE;
    mGatewayIP = NULL;
    mCheckInterval = 5;
    mInit = false;


    mOnlineChecker = new OnlineChecker();
    mOnlineChecker->setOnlineChecker(this);
}

Gateway::~Gateway()
{
    delete mOnlineChecker;
}

void Gateway::start()
{
    if(NULL==mGatewayIP)
    {
        return;
    }
    mOnlineChecker->setCheckInterval(mCheckInterval);
    mOnlineChecker->exec();
}

void Gateway::setStateChange(IGateway *stateChange)
{
    mStateChange = stateChange;
//    stateChanged();
}

void Gateway::setIp(char *ip)
{
    enter();
    mGatewayIP = ip;
    leave();
}

bool Gateway::execPing()
{
    Ping ping;
    return ping.exec(mGatewayIP, 3);
}

void Gateway::online()
{
    bool changed = false;
    enter();
    if((!mInit) || (GATEWAY_ONLINE != mState))
    {
        mState = GATEWAY_ONLINE;
        changed = true;
        if(!mInit)
        {
            mInit = true;
        }
    }
    leave();
    if(changed)
    {
        stateChanged();
    }
}

void Gateway::offline()
{
    bool changed = false;
    enter();
    if((!mInit) || (GATEWAY_OFFLINE != mState))
    {
        mState = GATEWAY_OFFLINE;
        changed = true;
        if(!mInit)
        {
            mInit = true;
        }
    }
    leave();
    if(changed)
    {
        stateChanged();
    }
}

void Gateway::checkOnline()
{
    bool success = execPing();
    if(success)
    {
        online();
    }
    else
    {
        offline();
    }
}

void Gateway::stateChanged()
{
    if(NULL != mStateChange)
    {
        mStateChange->gatewayStateChanged();
    }
}

