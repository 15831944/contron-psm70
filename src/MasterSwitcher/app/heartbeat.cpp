#include "heartbeat.h"

#include "sys/platform.h"

static char HEADER[] = {0x7A, 0xA7, 0xA7, 0x7A};
static int CRC_SIZE = 1;
static int LENGTH_SIZE = 1;
static int LENGTH_POS = sizeof(HEADER);
static int TIMEPOINT_POS = LENGTH_POS + 1;

Heartbeat::Heartbeat()
{
    mBufferSize = 0;
    mBuffer = NULL;
}

Heartbeat::~Heartbeat()
{
    if(isValid())
    {
        delete mBuffer;
    }
}

int Heartbeat::minSizeOfFrame()
{
    int size = sizeof(HEADER) + LENGTH_SIZE + CRC_SIZE;
    return size;
}

bool Heartbeat::foundHeader(char *buffer, int size)
{
    if(size<minSizeOfFrame())
    {
        return false;
    }
    char *p = buffer;
    bool found = (((*(p))==HEADER[0])
            &&((*(p+1))==HEADER[1])
            &&((*(p+2))==HEADER[2])
            &&((*(p+3))==HEADER[3])
            );
    return found;
}

void Heartbeat::setBuffer(char *buffer, int size)
{
    if(NULL==mBuffer)
    {
        mBuffer = (char *)malloc(size);
    }
    else
    {
        mBuffer = (char *)realloc(mBuffer, size);
    }
    if(NULL==mBuffer)
    {
        mBufferSize = 0;
        return;
    }
    memset(mBuffer, 0, size);
    memcpy(buffer, mBuffer, size);
    mBufferSize = size;
}

bool Heartbeat::makeFrame(char *buffer, int size)
{
    if(!foundHeader(buffer, size))
    {
        return false;
    }
    int len = *(buffer + LENGTH_POS);
    len--;  //去除crc长度
    char sum = 0;
    for(int i=0;i<len;i++)
    {
        sum += *(buffer + LENGTH_POS +i);
    }
    char crc = *(buffer + LENGTH_POS + len);
    if(sum != crc)
    {
        return false;
    }
    len++;
    setBuffer(buffer, LENGTH_POS+len);

    return isValid();
}

bool Heartbeat::makeBuffer(char **buffer, int &size)
{

}

bool Heartbeat::setData(bool isSlave, double timePoint)
{
    int len = minSizeOfFrame() + sizeof(UCHAR) + sizeof(double);
    UCHAR *buffer = (UCHAR *)malloc(len);
    if(NULL==buffer)
    {
        return false;
    }
    memset(buffer, 0, len);

    memcpy(buffer, HEADER, sizeof(HEADER));

    UCHAR slave = isSlave;
    buffer[LENGTH_POS] = slave;

    DOUBLE_CONVERTER t;
    t.d = timePoint;

    UCHAR *p = (UCHAR *)t.bytes;
    int size = sizeof(double);
    for(int i=0;i<size;i++)
    {
        *(buffer+TIMEPOINT_POS+i)=*(p++);
    }

    UCHAR sum = 0;
    for(int i=LENGTH_POS;i<len-1;i++)
    {
        sum += (UCHAR)(*(buffer+i));
    }


    *(buffer+len) = sum;
    setBuffer((char *)buffer, len);

    delete buffer;

    return isValid();
}

bool Heartbeat::isValid()
{
    return (mBuffer != NULL);
}

bool Heartbeat::getIsSlave()
{
    return false;
}

double Heartbeat::getTimePoint()
{
    return 0.0;
}

