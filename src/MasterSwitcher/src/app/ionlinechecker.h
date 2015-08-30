#ifndef IONLINECHECKER_H
#define IONLINECHECKER_H


class IOnlineChecker
{
public:
    IOnlineChecker();
    ~IOnlineChecker();

    virtual void checkOnline()=0;
};

#endif // IONLINECHECKER_H
