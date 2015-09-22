#ifndef IMUTEX_H
#define IMUTEX_H


class IMutex
{
public:
    IMutex();
    ~IMutex();

public:
    virtual void enter()=0;
    virtual void leave()=0;


};

#endif // IMUTEX_H
