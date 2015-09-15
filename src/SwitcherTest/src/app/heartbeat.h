#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "sys/platform.h"

class Heartbeat
{
public:
    Heartbeat();
    ~Heartbeat();

    bool makeFrame(char *buffer, int size);
    bool makeBuffer(char **buffer, int &size);
    bool setData(bool isSlave, UINT32 timePoint);

    bool isValid();

    double getTime();
    UINT32 getTimePoint();

    void print();

protected:
    int minSizeOfFrame();
    bool foundHeader(char *buffer, int size);
    void setBuffer(char *buffer, int size);

private:
    int mBufferSize;
    char *mBuffer;
};

#endif // HEARTBEAT_H
