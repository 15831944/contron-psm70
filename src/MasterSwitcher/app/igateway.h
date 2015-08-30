#ifndef IGATEWAY_H
#define IGATEWAY_H

class IGateway
{
public:
    IGateway();
    ~IGateway();

    virtual void gatewayStateChanged() = 0;
};

#endif // IGATEWAY_H
