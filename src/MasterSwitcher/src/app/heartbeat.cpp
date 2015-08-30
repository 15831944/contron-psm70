#include "heartbeat.h"

#include "sys/platform.h"
#include "utils.h"

static UCHAR HEADER[] = {0x7A, 0xA7, 0xA7, 0x7A};
static int CRC_SIZE = 1;
static int LENGTH_SIZE = 1;
static int LENGTH_POS = sizeof(HEADER);
static int STATE_POS = LENGTH_POS + 1;
static int TIMEPOINT_POS = STATE_POS + 1;

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
    bool found = ((UCHAR)((*(p))==HEADER[0])
            &&((UCHAR)(*(p+1))==HEADER[1])
            &&((UCHAR)(*(p+2))==HEADER[2])
            &&((UCHAR)(*(p+3))==HEADER[3])
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
    memcpy(mBuffer, buffer, size);
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
    if(!isValid())
    {
        return false;
    }

    *buffer = (char *)malloc(mBufferSize);
    if(NULL==*buffer)
    {
        return false;
    }
    char *p = *buffer;
    memset(p, 0, mBufferSize);
    memcpy(p, mBuffer, mBufferSize);
    size = mBufferSize;

    return true;
}

bool Heartbeat::setData(bool isSlave, double timePoint)
{
    int len = minSizeOfFrame() + sizeof(UCHAR) + sizeof(double);
    char *buffer = (char *)malloc(len);
    if(NULL==buffer)
    {
        return false;
    }
    memset(buffer, 0, len);

    int size = sizeof(HEADER);

    memcpy(buffer, HEADER, size);

    size = len - LENGTH_POS;
    buffer[LENGTH_POS] = size;


    UCHAR slave = isSlave;
    buffer[STATE_POS] = slave;

    DOUBLE_CONVERTER t;
    t.d = timePoint;

//    printf("value(%lf):%s\n", timePoint, buffer_format(t.bytes, sizeof(double)));

    UCHAR *p = (UCHAR *)t.bytes;
    size = sizeof(double);
    for(int i=0;i<size;i++)
    {
        *(buffer+TIMEPOINT_POS+i)=*(p++);
    }

    UCHAR sum = 0;
    for(int i=LENGTH_POS;i<len-1;i++)
    {
        sum += (UCHAR)(*(buffer+i));
    }

//    printf("sum:%02X\n", sum);


    *(buffer+len-1) = sum;

//    printf("buffer:%s\n", buffer_format(buffer, len));

    setBuffer(buffer, len);

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

