#ifndef COUNTER_H
#define COUNTER_H

#include <QDateTime>

#include <vector>
using namespace std;

typedef struct _ontest
{
    QDateTime lastTime;
    long lastSeed;
    QDateTime currentTime;
    long currentSeed;
} OneTest_t;

class Counter
{
public:
    Counter();
    ~Counter();

public:
    void receive(const QString &line);
    QString analytic();
    int max();
    int count();

protected:
    void test(QDateTime time, long seed);

protected:
    void reset();
    void clearData();

private:
    long mLastSeed;
    QDateTime mLastTime;
    vector<OneTest_t *> mData;
};

#endif // COUNTER_H
