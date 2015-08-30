#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "jk_crc.h"
#include "jk_function.h"
#include "jk_core.h"

class Jk_protocol_testTest : public QObject
{
    Q_OBJECT

public:
    Jk_protocol_testTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testJkProtocol_data();
    void testJkProtocol();
    void test_jk_crc();
    void test_jk_get_ushort();
    void test_jk_command_build();
    void test_jk_command_ack();
    void test_jk_find_frame();
};

Jk_protocol_testTest::Jk_protocol_testTest()
{
}

void Jk_protocol_testTest::initTestCase()
{
}

void Jk_protocol_testTest::cleanupTestCase()
{
}

void Jk_protocol_testTest::testJkProtocol_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void Jk_protocol_testTest::testJkProtocol()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void Jk_protocol_testTest::test_jk_crc()
{
    QString hexData = QString("A5 5A A5 5A 00 02 04 40 00 00 03 00 64 6E 04 00 B2 63");
    QByteArray data = QByteArray::fromHex(hexData.replace(" ", "").toLatin1());
    int size = data.length()-4-2;
    char *p = data.data();
    USHORT calcCRC = jk_crc_32((const char *)(p+4), size);
    UCHAR hi = 0xB2;
    UCHAR lo = 0x63;
    USHORT crc = (hi<<8) | lo;
    QCOMPARE(crc, calcCRC);

}

void Jk_protocol_testTest::test_jk_get_ushort()
{
    QByteArray data =  QByteArray::fromHex("B263");
    UCHAR hi = 0xB2;
    UCHAR lo = 0x63;
    USHORT crc = (hi<<8) | lo;
    char *p = data.data();

    USHORT calcCRC = jk_get_ushort((const char *)p);
    QCOMPARE(crc, calcCRC);


}

void Jk_protocol_testTest::test_jk_command_build()
{
    QString hexData = QString("A5 5A A5 5A 00 02 04 40 00 00 03 00 64 6E 04 00 B2 63");
    QByteArray data = QByteArray::fromHex(hexData.replace(" ", "").toLatin1());
    char *p = data.data();

    command_t *cmd = jk_command_build(p);

    QCOMPARE(cmd->data_size, (USHORT)2);
    QCOMPARE(cmd->protocol_type, (UCHAR)PRL_PT_JK);
    QCOMPARE(cmd->control_type, (UCHAR)PRL_CT_SEND);
    QCOMPARE(cmd->source_address, (USHORT)0);
    QCOMPARE(cmd->target_address, (USHORT)0x0300);
    QCOMPARE(cmd->source_type, (UCHAR)PRL_ST_YT);
    QCOMPARE(cmd->target_type, (UCHAR)PRL_ST_CONTRON);
    QCOMPARE(cmd->crc, (USHORT)0xB263);

//    QBENCHMARK {
//        jk_command_build(p);
//    }

}

void Jk_protocol_testTest::test_jk_command_ack()
{
    //ack
    //A5 5A A5 5A 00 02 04 81 03 00 00 00 6E 64 84 00 01 80 3B 51
    QString hexData = QString("A5 5A A5 5A 00 02 04 40 00 00 03 00 64 6E 04 00 B2 63");
    QByteArray data = QByteArray::fromHex(hexData.replace(" ", "").toLatin1());
    char *p = data.data();

    command_t *cmd = jk_command_build(p);
    command_t *ack = jk_command_ack(cmd);

    QString dataStr = QString(QByteArray(ack->data, ack->data_size).toHex());

    QVERIFY(cmd->protocol_type==ack->protocol_type);
    QVERIFY(ack->control_type==PRL_CT_ACK);
    QVERIFY(cmd->source_address==ack->target_address);
    QVERIFY(cmd->target_address==ack->source_address);
    QVERIFY(cmd->source_type==ack->target_type);
    QVERIFY(cmd->target_type==ack->source_type);
    QVERIFY(ack->data_size==4);
    QVERIFY(dataStr==QString("84 00 01 80").replace(" ", ""));

}

void Jk_protocol_testTest::test_jk_find_frame()
{
    QString frame1 = QString("A5 5A A5 5A 00 02 04 40 00 00 03 00 64 6E 04 00 B2 63").replace(" ", "");
    QString frame2 = QString("A5 5A A5 5A 00 03 04 40 00 00 03 00 64 6E 02 00 01 2B 2E").replace(" ", "");
    QByteArray data;
    data.append(QByteArray::fromHex(frame1.toLatin1()));
    data.append(QByteArray::fromHex(frame2.toLatin1()));
    int pos = 0;
    int len = data.length();
    char *p = data.data();
    int frame_pos, frame_len;
    int count = 0;
//    {
//        QString str;
//        QDebug d(&str); d<<"test_jk_find_frame:len="<<len;
//        QWARN(str.toLocal8Bit().data());
//    }
    while(1==jk_find_frame(p, len, pos, frame_pos, frame_len))
    {
        count++;
        QByteArray t(p+frame_pos, frame_len);
        QString str = QString(t.toHex());
        qDebug() << "test_jk_find_frame(" << count << "):" << str;
        if(1==count)
        {
            QVERIFY(str==frame1);
        }
        if(2==count)
        {
            QVERIFY(str==frame2);
        }
    }
//    {
//        QString str;
//        QDebug d(&str); d<<"test_jk_find_frame:pos="<<pos;
//        QWARN(str.toLocal8Bit().data());
//    }

}

QTEST_MAIN(Jk_protocol_testTest)

#include "tst_jk_protocol_testtest.moc"
