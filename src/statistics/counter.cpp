#include "counter.h"

#include <QStringList>
#include <QDebug>

Counter::Counter()
{
    reset();
}

Counter::~Counter()
{

}

void Counter::receive(const QString &line)
{
    QString t = line.trimmed().replace("\t", " ");
    t = t.replace("  ", " ");
    t = t.replace("\n", "");
    if(t.isEmpty())
    {
        return;
    }
    QStringList list = t.split(" ");
    QStringList sl;
    for(int i=0;i<list.count();i++)
    {
        QString str = list.at(i).trimmed();
        if(!str.isEmpty())
        {
            sl.append(str);
        }
    }
    if(3!=sl.count())
    {
        return;
    }
    QString timeStr = QString("%1 %2").arg(sl.at(0).trimmed()).arg(sl.at(1).trimmed());
    int pos = timeStr.indexOf(".");
    timeStr = timeStr.mid(0, pos);
    bool succes;
//    QString zStr = timeStr.mid(pos);
//    long z = zStr.toLong(&succes);
//    if(!succes)
//    {
//        return;
//    }
    QString timeFormat = "yyyy-MM-dd hh:mm:ss";
//    int len = timeStr.length()-timeFormat.length();
//    timeFormat = timeFormat.append(QString("z").repeated(len));
    QDateTime time = QDateTime::fromString(timeStr, timeFormat);
    if(!time.isValid())
    {
        return;
    }
    long seed = sl.at(2).trimmed().toLong(&succes);
    if(!succes)
    {
        return;
    }
    test(time, seed);

}

void Counter::test(QDateTime time, long seed)
{
    if(0==mLastSeed)
    {
        mLastSeed = seed;
        mLastTime = time;
        return;
    }
    if(seed < mLastSeed)
    {
        reset();
        mLastSeed = seed;
        mLastTime = time;
        return;
    }
    if(seed-1!=mLastSeed)
    {
        OneTest_t *t = (OneTest_t *)malloc(sizeof(OneTest_t));
        if(NULL==t)
        {
            printf("found:\nstart time:%s\nend time:%s\nstart:%ld\nend:%ld\n",
                   mLastTime.toString("yyyy-MM-dd hh:mm:ss:zzz").toLocal8Bit().data(),
                   time.toString("yyyy-MM-dd hh:mm:ss:zzz").toLocal8Bit().data(),
                   mLastSeed,
                   seed);
            return;
        }
        memset(t, 0, sizeof(OneTest_t));
        t->currentSeed = seed;
        t->currentTime = time;
        t->lastSeed = mLastSeed;
        t->lastTime = mLastTime;
        mData.push_back(t);
        printf("found:\nstart time:%s\nend time:%s\nstart:%ld\nend:%ld\n",
               mLastTime.toString("yyyy-MM-dd hh:mm:ss:zzz").toLocal8Bit().data(),
               time.toString("yyyy-MM-dd hh:mm:ss:zzz").toLocal8Bit().data(),
               mLastSeed,
               seed);
    }
    mLastSeed = seed;
    mLastTime = time;
}

QString Counter::analytic()
{
    QString result;
    result.append(QString("Test count: %1\n").arg(mData.size()));
    vector<OneTest_t *>::iterator i = mData.begin();
    while(i!=mData.end())
    {
        OneTest_t *t = *i;
        result.append("--------------------------\n");
        result.append(QString("from: %1\n").arg(t->lastTime.toString("yyyy-MM-dd hh:mm:ss:zzz")));
        result.append(QString("to  : %1\n").arg(t->currentTime.toString("yyyy-MM-dd hh:mm:ss:zzz")));
        result.append(QString("time: %1\n").arg(t->currentSeed-t->lastSeed));
        i++;
    }
    return result;
}

int Counter::max()
{
    int max = 0;
    vector<OneTest_t *>::iterator i = mData.begin();
    while(i!=mData.end())
    {
        OneTest_t *t = *i;
        int time = t->currentSeed-t->lastSeed;
        if(time>max)
        {
            max = time;
        }
        i++;
    }

    return max;
}

int Counter::count()
{
    return mData.size();
}

void Counter::reset()
{
    mLastSeed = 0;
    clearData();
}

void Counter::clearData()
{
    vector<OneTest_t *>::iterator i = mData.begin();
    while(i!=mData.end())
    {
        OneTest_t *t = *i;
        delete t;
        i = mData.erase(i);
    }
}

