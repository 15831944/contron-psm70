#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "utils.h"
#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "log.h"
#include <QClipboard>

static MainWindow *self = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    BaseObject(),
//    ITcpServer(),
//    ITcpClient(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(log(QString)), this, SLOT(writeLog(QString)), Qt::QueuedConnection);


    ui->lineEdit->setText("127.0.0.1");
    mServer = new LocalPC();
    mServer->setUserLock(this);
//    mServer->setHandler(this);

    mClient = NULL;

}

MainWindow::~MainWindow()
{
    if(NULL!=mClient)
    {
        delete mClient;
        mClient = NULL;
    }
//    mServer->close();
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
    UN_USE(tcp);
    enter();
//    ITcpClient **handler = (ITcpClient **)(tcp);
//    *handler = this;
    leave();
}

void MainWindow::tcpClientReceiveData(void *tcp, char *buffer, int size)
{
    enter();

    APP_LOG("receive:\t%s\n", buffer_format(buffer, size));
    HeartbeatProtocol protocol;
    Heartbeat *hb = protocol.find(buffer, size);
    if(hb!=NULL)
    {
        delete hb;

        TcpClient *client = (TcpClient *)tcp;

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
    leave();
}

void MainWindow::tcpClientConnected(void *tcp)
{
    UN_USE(tcp);
}

void MainWindow::tcpClientDisconnected(void *tcp)
{
    TcpClient *client = (TcpClient *)tcp;
    delete client;
}

void MainWindow::tcpClientError(void *tcp)
{
    UN_USE(tcp);
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
    enter();
    mServer->setPort(ui->spinBox->value());
    mServer->start();
    leave();
}

void MainWindow::on_radioButton_2_clicked()
{
    enter();
    if(NULL!=mClient)
    {
        mClient->setEnableReconnect(ui->radioButton->isChecked());
    }
    leave();
}

void MainWindow::on_pushButton_2_clicked()
{
    enter();
    if(NULL==mClient)
    {
        mClient = new RemotePC();
        mClient->setLocalPC(this->mServer);
        QString ip = ui->lineEdit->text();
        QByteArray ba = ip.toLocal8Bit();
        char *p = ba.data();
        mClient->setIp(p);
        mClient->setPort(ui->spinBox_2->value());
        mClient->setEnableReconnect(ui->radioButton->isChecked());
        mClient->setUserLock(this);
        mClient->start();
    }
    leave();
}

void MainWindow::on_pushButton_3_clicked()
{
    enter();
    if(NULL!=mClient)
    {
        mClient->close();
        delete mClient;
        mClient = NULL;
    }
    leave();
}

void MainWindow::on_pushButton_5_clicked()
{
    enter();
    ui->textEdit->clear();
    leave();
}

void MainWindow::on_pushButton_4_clicked()
{
    enter();
    QClipboard *clipboard = QApplication::clipboard();
    QString newText = ui->textEdit->toPlainText();
    clipboard->setText(newText);
    leave();
}
