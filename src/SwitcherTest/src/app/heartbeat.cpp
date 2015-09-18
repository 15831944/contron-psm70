#include "heartbeat.h"

#include "sys/platform.h"
#include "utils.h"
#include "log.h"

static UCHAR HEADER[] = {0x8A, 0xA8, 0xA8, 0x8A};
static int CRC_SIZE = 1;
static int LENGTH_SIZE = 1;
static int LENGTH_POS = sizeof(HEADER);
static int TIME_SIZE = sizeof(struct timeval);
static int TIME_POS = LENGTH_POS + 1;
static int TIMEPOINT_POS = TIME_POS + TIME_SIZE;

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
    bool found = (((UCHAR)(*p)==HEADER[0])
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

void Heartbeat::print()
{
    if(!isValid())
    {
        return;
    }
    char strTime[128];
    memset(strTime, 0, 128);

    double time = getTime();
    UINT32 timePoint = getTimePoint();


    struct timeval tv;
    TIME_TO_STRUCT(time, tv);

   struct tm *ptm = localtime(&tv.tv_sec);  //将秒转换成struct tm的形式
   strftime(strTime, 128, "%Y-%m-%d %H:%M:%S", ptm);
   printf("now=%s.%ld\n", strTime, tv.tv_usec); //输出精确到微秒


    APP_LOG("%s.%ld\t\t\t%d\n", strTime, tv.tv_usec, timePoint);
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

bool Heartbeat::setData(bool isSlave, UINT32 timePoint)
{
    UN_USE(isSlave);
    int len = minSizeOfFrame() + TIME_SIZE + sizeof(UINT32);
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

    double now;
    GET_TIME(now);
    struct timeval tv;
    TIME_TO_STRUCT(now, tv);


    TIMEVAL_CONVERTER tc;
    tc.tv = tv;
    memcpy(&buffer[TIME_POS], &tc.bytes, TIME_SIZE);

    UINT32_CONVERTER t;
    t.i = timePoint;
    memcpy(&buffer[TIMEPOINT_POS], &t.bytes, sizeof(t.bytes));

//    UCHAR *p = (UCHAR *)t.bytes;
//    size = sizeof(t.bytes);
//    for(int i=0;i<size;i++)
//    {
//        *(buffer+TIMEPOINT_POS+i)=*(p++);
//    }

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

double Heartbeat::getTime()
{
    double result = 0.0;
    if(isValid())
    {
        TIMEVAL_CONVERTER c;
        int size = sizeof(c.bytes);
        memcpy(&c.bytes, &mBuffer[TIME_POS], size);
        STRUCT_TO_TIME(result, c.tv);
    }
    return result;
}

UINT32 Heartbeat::getTimePoint()
{
    UINT32 result = 0;
    if(isValid())
    {
        UINT32_CONVERTER c;
        int size = sizeof(c.bytes);
        memcpy(&c.bytes, &mBuffer[TIMEPOINT_POS], size);
        result = c.i;
    }
    return result;
}

