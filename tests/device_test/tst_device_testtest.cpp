#include <QString>
#include <QtTest>

#include "psm_device.h"

class Device_testTest : public QObject
{
    Q_OBJECT

public:
    Device_testTest();

private Q_SLOTS:
    void testCase1();
};

Device_testTest::Device_testTest()
{
}

void Device_testTest::testCase1()
{
    char str1[] = "device#1\0";
    char str2[] = "device#2\0";
    char str3[] = "device#3\0";
    char str4[] = "device#4\0";

    device_t *list = NULL;
    device_insert(&list, str1, strlen(str1));
    device_insert(&list, str2, strlen(str2));
    device_insert(&list, str3, strlen(str3));
    device_insert(&list, str4, strlen(str4));

    device_remove(&list, str1, strlen(str1));

    device_t *next = list;
    while(NULL!=next)
    {
        QWARN(next->name);
        next = (device_t *)next->next;
    }
}

QTEST_APPLESS_MAIN(Device_testTest)

#include "tst_device_testtest.moc"
