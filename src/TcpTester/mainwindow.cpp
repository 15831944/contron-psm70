#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "utils.h"
#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "log.h"

static MainWindow *self = NULL;

#define MAX_MISSED_HEARTBEAT 3


THREAD_API remote_heartbeat_thread(void *param)
{
    MainWindow *remote = (MainWindow *)param;

    int idle = 300;
    int count = 1;
    while(true)
    {
        Sleep(10);
        THREAD_WAITEXIT();

        bool connected = remote->isConnected();
        if(!connected)
        {
            break;  //外部断开连接
        }

        //1.快速发送心跳count次，确认连接，间隔1秒
        if(count)
        {
            count--;

            APP_LOG("[RemotePC]fast heartbeat: %d\n", count);
            remote->heartbeat();

            if(!count)
            {
                remote->updateSendTime();
            }

            int fast_interval = 1000;
            Sleep(fast_interval);
            continue;
        }
        //4.检测心跳发送三次无回应
        {
            bool send_error = remote->isSendFail();
            if(send_error)
            {
                remote->tryBreakConnection();
                break;
            }
        }
        //3.定时发送心跳
        {
            bool outOfTime = remote->isSendTime();
            if(outOfTime)
            {
                remote->heartbeat();
                remote->updateSendTime();
            }
        }
        Sleep(idle);
    }//while
    APP_LOG("[RemotePC]heartbeat thread exit\n");
    return NULL;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    BaseObject(),
    ITcpServer(),
    ITcpClient(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(log(QString)), this, SLOT(writeLog(QString)), Qt::QueuedConnection);

    mSendTime = 0;
    mHeartbeatCount = 0;
    mSendInterval = 0;
    mConnectCount = 0;

    mServer = new TcpServer();
    mServer->setHandler(this);

    mClient = new TcpClient();
    mClient->setHandler(this);
}

MainWindow::~MainWindow()
{
    mServer->close();
    mClient->close();

    delete mClient;
    delete mServer;

    delete ui;
}

void MainWindow::setSingletone(MainWindow *win)
{
    self = win;
}

MainWindow *MainWindow::getSingletone()
{
    return self;
}

void MainWindow::addNewClient(void *tcp)
{
    enter();
    TcpClient *client = (TcpClient *)tcp;
    client->setHandler(this);
    leave();
}

void MainWindow::tcpClientReceiveData(void *tcp, char *buffer, int size)
{
    enter();

    APP_LOG("receive:\n%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        delete hb;
        TcpClient *client = (TcpClient *)tcp;
        if(client!=mClient)
        {
            char *p = NULL;
            int size = 0;
            bool isSlave = false;
            double timePoint;
            GET_TIME(timePoint);
            Heartbeat *t = protocol.makeHeartbeat(isSlave, timePoint);
            if(NULL!=t)
            {
                if(t->makeBuffer(&p, size))
                {
                    client->send(p, size);
                    delete p;
                }
            }
        }
        else
        {
            clearHeartbeatCount();
        }
    }
    leave();

}

void MainWindow::tcpClientConnected(void *tcp)
{
    TcpClient *client = (TcpClient *)tcp;
    enter();
    if(client!=mClient)
    {
        delete client;
    }
    else
    {
        clearConnectCount();
        enableHeartbeat();
    }
    leave();
}

void MainWindow::tcpClientDisconnected(void *tcp)
{
    TcpClient *client = (TcpClient *)tcp;
    enter();
    if(client!=mClient)
    {
        delete client;
    }
    else
    {
        disableHeartbeat();
    }
    leave();
}

void MainWindow::tcpClientError(void *tcp)
{

}

bool MainWindow::isConnected()
{
    bool result = false;
    enter();
    result = mClient->isConnected();
    leave();
    return result;
}

void MainWindow::heartbeat()
{
    enter();

    mHeartbeatCount++;
    char *p = NULL;
    int size = 0;
    HeartbeatProtocol protocol;
    bool isSlave = false;
    double timePoint;
    GET_TIME(timePoint);
    Heartbeat *hb = protocol.makeHeartbeat(isSlave, timePoint);
    if(NULL!=hb)
    {
        if(hb->makeBuffer(&p, size))
        {
            mClient->send(p, size);
            delete p;
        }
    }

    leave();
}

void MainWindow::updateSendTime()
{
    enter();
    GET_TIME(mSendTime);
    APP_LOG("[RemotePC]update send time:%d\n", mSendTime);
    leave();
}

bool MainWindow::isSendFail()
{
    bool result = true;

    enter();
    APP_LOG("[RemotePC]is send fail:%d\n", mHeartbeatCount);
    result = (MAX_MISSED_HEARTBEAT<=mHeartbeatCount);
    leave();

    return result;
}

void MainWindow::tryBreakConnection()
{
    enter();
    mClient->tryBreakConnection();
    leave();
}

bool MainWindow::isSendTime()
{
    bool result = false;

    enter();
    if(0!=mSendTime)
    {
        int now;
        GET_TIME(now);

        result = (now>=(mSendTime+mSendInterval));
    }
    leave();

    return result;
}

void MainWindow::clearConnectCount()
{
    enter();
    mConnectCount = 0;
    leave();
}

void MainWindow::clearHeartbeatCount()
{
    enter();
    mHeartbeatCount = 0;
    leave();
}

void MainWindow::enableHeartbeat()
{
    enter();
    mHeartbeatCount = 0;
    mSendTime = 0;
    int ret;
    THREAD_CREATE(&mHeartbeatThread, remote_heartbeat_thread, this, ret);
    if(ret)
    {
        THREAD_RUN(mHeartbeatThread, false);
    }
    leave();
}

void MainWindow::disableHeartbeat()
{
    enter();
    THREAD_CLOSE(mHeartbeatThread);
    leave();
}

void MainWindow::writeLog(const QString &text)
{
    enter();
    QString str = text;
    if(str.mid(0, 1)=="\"")
    {
        str = str.mid(1);
    }
    if(str.mid(str.length()-1)=="\"")
    {
        str = str.mid(0, str.length() - 1);
    }
//    str = str.replace("\"", "");
    ui->textEdit->append(str);
    leave();
}

void MainWindow::on_pushButton_clicked()
{
    mServer->setPort(ui->spinBox->value());
    mServer->start();
}

void MainWindow::on_radioButton_2_clicked()
{
    mClient->setEnableReconnect(ui->radioButton->isChecked());
}

void MainWindow::on_pushButton_2_clicked()
{
    QString ip = ui->lineEdit->text();
    QByteArray ba = ip.toLocal8Bit();
    char *p = ba.data();
    mClient->setIp(p);
    mClient->setPort(ui->spinBox_2->value());
    mClient->start(true);
}
