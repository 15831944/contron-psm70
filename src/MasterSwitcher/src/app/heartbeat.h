#ifndef HEARTBEAT_H
#define HEARTBEAT_H


class Heartbeat
{
public:
    Heartbeat();
    ~Heartbeat();

    bool makeFrame(char *buffer, int size);
    bool makeBuffer(char **buffer, int &size);
    bool setData(bool isSlave, double timePoint);

    bool isValid();

    bool getIsSlave();
    double getTimePoint();

protected:
    int minSizeOfFrame();
    bool foundHeader(char *buffer, int size);
    void setBuffer(char *buffer, int size);

private:
    int mBufferSize;
    char *mBuffer;
};

#endif // HEARTBEAT_H
