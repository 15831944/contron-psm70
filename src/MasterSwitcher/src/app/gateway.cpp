#include "gateway.h"

#include "sys/platform.h"

#include "ping.h"

Gateway::Gateway()
    :BaseObject(), IOnlineChecker()
{
    mState = GATEWAY_ONLINE;
    mGatewayIP = "10.7.5.254";

    mOnlineChecker = new OnlineChecker();
    mOnlineChecker->setOnlineChecker(this);
    mOnlineChecker->setCheckInterval(1);
}

Gateway::~Gateway()
{
    delete mOnlineChecker;
}

void Gateway::start()
{
    mOnlineChecker->exec();
}

void Gateway::setStateChange(IGateway *stateChange)
{
    mStateChange = stateChange;
    stateChanged();
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
    if(GATEWAY_ONLINE != mState)
    {
        mState = GATEWAY_ONLINE;
        changed = true;
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
    if(GATEWAY_OFFLINE != mState)
    {
        mState = GATEWAY_OFFLINE;
        changed = true;
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

