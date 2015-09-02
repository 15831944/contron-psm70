#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "utils.h"
#include "heartbeatprotocol.h"
#include "heartbeat.h"

#include "log.h"

static MainWindow *self = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    BaseObject(),
    ITcpServer(),
//    ITcpClient(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(log(QString)), this, SLOT(writeLog(QString)), Qt::QueuedConnection);


    mServer = new TcpServer();
    mServer->setHandler(this);

    mClient = NULL;

}

MainWindow::~MainWindow()
{
    if(NULL!=mClient)
    {
        delete mClient;
        mClient = NULL;
    }
    mServer->close();
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
//    enter();
//    ITcpClient **handler = (ITcpClient **)(tcp);
//    *handler = this;
//    leave();
}

void MainWindow::tcpClientReceiveData(void *tcp, char *buffer, int size)
{

}

void MainWindow::tcpClientConnected(void *tcp)
{
//    TcpClient *client = (TcpClient *)tcp;
//    enter();

//    delete client;
//    leave();
}

void MainWindow::tcpClientDisconnected(void *tcp)
{
//    TcpClient *client = (TcpClient *)tcp;
//    enter();
//    leave();
//    delete client;
}

void MainWindow::tcpClientError(void *tcp)
{

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
    mClient->setEnableReconnect(ui->radioButton->isChecked());
    leave();
}

void MainWindow::on_pushButton_2_clicked()
{
    enter();
    if(NULL==mClient)
    {
        mClient = new RemotePC();
        QString ip = ui->lineEdit->text();
        QByteArray ba = ip.toLocal8Bit();
        char *p = ba.data();
        mClient->setIp(p);
        mClient->setPort(ui->spinBox_2->value());
        mClient->start();
    }
    leave();
}

void MainWindow::on_pushButton_3_clicked()
{
    enter();
    if(NULL!=mClient)
    {
        delete mClient;
        mClient = NULL;
    }
    leave();
}
