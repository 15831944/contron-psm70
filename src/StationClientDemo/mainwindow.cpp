#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "psm_def.h"
#include "stationlibrary.h"

#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>


static MainWindow *self = NULL;

void server_request_data(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站召唤全状态,站号:%1").arg(station_no));

}

void client_request_data(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站召唤全状态,站号:%1").arg(station_no));

}

void server_data_change(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站变位遥信,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QString("%1,%2").arg(QString(d->device)).arg(d->state_1));
    }
}

void client_data_change(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站变位遥信,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QString("%1,%2").arg(QString(d->device)).arg(d->state_1));
    }
}

void app_receive_ticket(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("接收到操作票"));
    emit self->logChange(QObject::tr("站号:%1").arg(station_no));
    emit self->logChange(QObject::tr("票项数:%1").arg(list->count));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("票项%1: 设备主编号=%2 操作前状态=%3 操作后状态=%4")
                             .arg(i+1).arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
}

void server_lock_data_change(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站唯一权,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QString("%1,%2").arg(QString(d->device)).arg(d->state_2));
    }
}

void client_lock_data_change(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站唯一权,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QString("%1,%2").arg(QString(d->device)).arg(d->state_2));
    }
}

void server_send_lock_device(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求闭锁,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2")
                             .arg(QString(d->device)).arg(d->state_1));
    }
    jk2sub_lock_device(station_no, list);
}

void server_send_unlock_device(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求解锁,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2")
                             .arg(QString(d->device)).arg(d->state_1));
    }
    jk2sub_unlock_device(station_no, list);
}

void server_send_unlock_device_all(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站向子站请求解锁所有设备,站号:%1").arg(station_no));
    jk2sub_unlock_all_device(station_no);
}

void server_send_lock_device_all(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站向子站请求闭锁所有设备,站号:%1").arg(station_no));
    jk2sub_lock_all_device(station_no);
}

void client_send_lock_device_all_result(int station_no, int result)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送设备全闭锁结果,站号:%1，结果：%2")
                         .arg(station_no).arg(result));

    jk2main_lock_all_device_result(station_no, result);
}

void client_send_unlock_device_all_result(int station_no, int result)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送设备全解锁结果,站号:%1，结果：%2")
                         .arg(station_no).arg(result));

    jk2main_unlock_all_device_result(station_no, result);
}

void client_unlock_device_result(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送解锁结果,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2 结果=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }

    jk2main_unlock_device_result(station_no, list);
}

void client_lock_device_result(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送闭锁结果,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2 结果=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }

    jk2main_lock_device_result(station_no, list);

}

void jk2sub_receive_ticket(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("接收到主站操作票,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作前状态=%2 操作后状态=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
}

void main2jk_send_ticket(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("接收到主站操作票,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作前状态=%2 操作后状态=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
    jk2sub_send_ticket(station_no, list);
}

void sub2jk_send_yk_device_state(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站上报遥控闭锁继电器状态,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 继电器类型=%2 断电器状态=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
    jk2main_yk_device_state(station_no, list);
}

void server_yk_device_state(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站上报遥控闭锁继电器状态,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 继电器类型=%2 断电器状态=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
}

void server_lock_device_result(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送闭锁结果,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2 操作结果=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
}

void server_unlock_device_result(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送解锁结果,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2 操作结果=%3")
                             .arg(QString(d->device)).arg(d->state_1).arg(d->state_2));
    }
}

void server_lock_device_all_result(int station_no, int result)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送全闭锁结果,站号:%1，结果：%2")
                         .arg(station_no).arg(result));
}

void server_unlock_device_all_result(int station_no, int result)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站向主站发送全解锁结果,站号:%1，结果：%2")
                         .arg(station_no).arg(result));
}

void app_request_lock_device(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求设备闭锁,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2")
                             .arg(QString(d->device)).arg(d->state_1));
    }
}

void app_request_unlock_device(int station_no, jk_device_list_t *list)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求设备解锁,站号:%1").arg(station_no));
    for(int i=0;i<list->count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        emit self->logChange(QObject::tr("设备主编号=%1 操作=%2")
                             .arg(QString(d->device)).arg(d->state_1));
    }
}

void main2jk_send_request_sync_time(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求时间同步，站号：%1").arg(station_no));

    jk2main_request_sync_time(station_no);
}

void sub2jk_send_request_sync_time(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站请求时间同步，站号：%1").arg(station_no));

    jk2sub_request_sync_time(station_no);
}

void server_request_sync_time(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站请求时间同步，站号：%1").arg(station_no));
}

void client_request_sync_time(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求时间同步，站号：%1").arg(station_no));
}

void client_lock_device_all(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求闭锁所有设备，站号：%1").arg(station_no));
}

void client_unlock_device_all(int station_no)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站请求解锁所有设备，站号：%1").arg(station_no));
}

void main2jk_send_sync_time(int station_no, jk_time_t *time)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站时间同步，站号：%1").arg(station_no));
    emit self->logChange(QObject::tr("时间：%1年%2月%3日%4时%5分%6秒")
                         .arg(time->year).arg(time->month).arg(time->day)
                         .arg(time->hour).arg(time->minute).arg(time->second));

    jk2sub_reply_sync_time(station_no, time);
}

void sub2jk_send_sync_time(int station_no, jk_time_t *time)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站时间同步，站号：%1").arg(station_no));
    emit self->logChange(QObject::tr("时间：%1年%2月%3日%4时%5分%6秒")
                         .arg(time->year).arg(time->month).arg(time->day)
                         .arg(time->hour).arg(time->minute).arg(time->second));

    jk2main_reply_sync_time(station_no, time);
}

void server_sync_time(int station_no, jk_time_t *time)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("子站时间同步，站号：%1").arg(station_no));
    emit self->logChange(QObject::tr("时间：%1年%2月%3日%4时%5分%6秒")
                         .arg(time->year).arg(time->month).arg(time->day)
                         .arg(time->hour).arg(time->minute).arg(time->second));
}

void client_sync_time(int station_no, jk_time_t *time)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QObject::tr("主站时间同步，站号：%1").arg(station_no));
    emit self->logChange(QObject::tr("时间：%1年%2月%3日%4时%5分%6秒")
                         .arg(time->year).arg(time->month).arg(time->day)
                         .arg(time->hour).arg(time->minute).arg(time->second));
}

void dll_log(char *str)
{
    if(NULL==self)
    {
        return;
    }
    emit self->logChange(QString(str));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);
    ui->pushButton_8->setVisible(false);
    ui->pushButton_9->setVisible(false);
    ui->pushButton_20->setVisible(false);
    ui->pushButton_21->setVisible(false);
    connect(this, SIGNAL(logChange(QString)),
            this, SLOT(log(QString)), Qt::QueuedConnection);
    self = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(const QString &text)
{
    QString str = QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
    str = QString("%1    %2").arg(str).arg(text);
    ui->textEdit->append(str);
}

void MainWindow::setButtonEnable(bool enabled)
{
    QList<QPushButton *> list = this->findChildren<QPushButton *>();
    for(int i=0;i<list.count();i++)
    {
        list.at(i)->setEnabled(enabled);
    }
}

void MainWindow::setCommonButtonEnable()
{
    QStringList list;
    list << "pushButton_3"
         << "pushButton_4";
    for(int i=0;i<list.count();i++)
    {
        QPushButton *button = this->findChild<QPushButton *>(list.at(i));
        if(button)
        {
            button->setEnabled(true);
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    //解锁结果
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0,1;201=1,2;301=0,1";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    int size = count*sizeof(jk_device_t);
    devices.device = (jk_device_t *)malloc(size);
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, size);

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).split(",").at(0).trimmed().toInt();
        int result = pair.at(1).split(",").at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
        d->state_2 = result;
    }
    app_send_unlock_device_result(station_no, &devices);
    free(devices.device);
}

void MainWindow::on_pushButton_2_clicked()
{
    //闭锁结果
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0,1;201=1,2;301=0,1";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    int size = count*sizeof(jk_device_t);
    devices.device = (jk_device_t *)malloc(size);
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, size);

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).split(",").at(0).trimmed().toInt();
        int result = pair.at(1).split(",").at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
        d->state_2 = result;
    }
    app_send_lock_device_result(station_no, &devices);
    free(devices.device);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->lineEdit->clear();
//    QSharedMemory sm("s_c");
//    if(sm.isAttached() or sm.attach())
//    {
//        QBuffer buffer;
//        QDataStream in(&buffer);
//        sm.lock();
//        buffer.setData((char*)sm.constData(), sm.size());
//        buffer.open(QBuffer::ReadOnly);
//        int type;
//        in >> type;
//        qDebug() << type;
//        switch (type) {
//        case 1:
//            int value;
//            in >> value;

//            qDebug() << value;

//            break;
//        default:
//            break;
//        }

//        sm.unlock();
//        sm.detach();
//        return;
//    }
//    qDebug() << sm.error();

}

void MainWindow::on_pushButton_4_clicked()
{
    int mode = MODE_SERVER;
    if(ui->appRadioButton->isChecked())
    {
        mode = MODE_SUBSTATION;
    }
    if(ui->appRadioButton_2->isChecked())
    {
        mode = MODE_MAINSTATION;
    }
    setButtonEnable(false);
    setCommonButtonEnable();
    dll_init(mode);
    set_log(dll_log);
    QString name = "substation";
    switch (mode) {
    case MODE_SERVER:
    {
        //server
        name = tr("服务");
        set_sub2jk_request_data(client_request_data);
        set_client_data_change(client_data_change);
        set_client_lock_data_change(client_lock_data_change);
        set_sub2jk_send_unlock_device_result(client_unlock_device_result);
        set_sub2jk_send_lock_device_result(client_lock_device_result);
        set_main2jk_request_data(server_request_data);
        set_server_data_change(server_data_change);
        set_server_lock_data_change(server_lock_data_change);
        set_sub2jk_send_request_sync_time(sub2jk_send_request_sync_time);
        set_main2jk_send_request_sync_time(main2jk_send_request_sync_time);
        set_main2jk_send_reply_sync_time(main2jk_send_sync_time);
        set_sub2jk_send_reply_sync_time(sub2jk_send_sync_time);
        set_main2jk_send_ticket(main2jk_send_ticket);
        set_sub2jk_send_yk_device_state(sub2jk_send_yk_device_state);
        set_main2jk_send_lock_device(server_send_lock_device);
        set_main2jk_send_unlock_device(server_send_unlock_device);
        set_main2jk_send_lock_all_device(server_send_lock_device_all);
        set_main2jk_send_unlock_all_device(server_send_unlock_device_all);
        set_sub2jk_send_lock_all_device_result(client_send_lock_device_all_result);
        set_sub2jk_send_unlock_all_device_result(client_send_unlock_device_all_result);
    }
        break;
    case MODE_SUBSTATION:
    {
        //client
        QStringList list;
        list << "pushButton_18"
             << "pushButton"
             << "pushButton_2"
             << "pushButton_15"
             << "pushButton_17"
             << "pushButton_10";
        for(int i=0;i<list.count();i++)
        {
            QPushButton *button = this->findChild<QPushButton *>(list.at(i));
            if(button)
            {
                button->setEnabled(true);
            }
        }
        name = tr("子站");
        set_substation_receive_ticket(jk2sub_receive_ticket);
        set_substation_unlock_device(app_request_unlock_device);
        set_substation_lock_device(app_request_lock_device);
        set_substation_sync_time(client_sync_time);
        set_substation_request_sync_time(client_request_sync_time);
        set_substation_lock_all_device(client_lock_device_all);
        set_substation_unlock_all_device(client_unlock_device_all);
    }
        break;
    case MODE_MAINSTATION:
    {
        //mainstation
        QStringList list;
        list << "pushButton_12"
             << "pushButton_11"
             << "pushButton_13"
             << "pushButton_14"
             << "pushButton_16"
             << "pushButton_19";
        for(int i=0;i<list.count();i++)
        {
            QPushButton *button = this->findChild<QPushButton *>(list.at(i));
            if(button)
            {
                button->setEnabled(true);
            }
        }
        name = tr("主站");
        set_mainstation_sync_time(server_sync_time);
        set_mainstation_request_sync_time(server_request_sync_time);
        set_mainstation_yk_device_state(server_yk_device_state);
        set_mainstation_lock_device_result(server_lock_device_result);
        set_mainstation_unlock_device_result(server_unlock_device_result);
        set_mainstation_lock_device_all_result(server_lock_device_all_result);
        set_mainstation_unlock_device_all_result(server_unlock_device_all_result);

    }
        break;
    default:
        break;
    }
    this->setWindowTitle(name);
    log("I'm "+name+".");
}

void MainWindow::on_pushButton_5_clicked()
{
    //变位遥信
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0;201=1;301=0";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    devices.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, count*sizeof(jk_device_t));

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
    }
    app_data_change(station_no, &devices);
    free(devices.device);
}

void MainWindow::on_pushButton_6_clicked()
{
    //全遥信
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1";
        ui->lineEdit->setText(str);
    }
    int station = str.toInt();
    app_request_data(station);
}

void MainWindow::on_pushButton_7_clicked()
{
    //唯一权
    /*
    QByteArray ba = QByteArray::fromHex("0200010003033130310003323031010333303100");
    QBuffer buffer;
    buffer.setData(ba);
    buffer.open(QBuffer::ReadOnly);
    uchar cmd = 0;
    ushort no = 0;
    ushort count = 0;
    QDataStream in(&buffer);
    in >> cmd;
    in >> no;
    in >> count;
    qDebug()<<"cmd="<<cmd<<",station="<<no<<",count="<<count;
    */

    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0;201=1;301=0";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    devices.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, count*sizeof(jk_device_t));

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_2 = state;
    }
    app_lock_data_change(station_no, &devices);
    free(devices.device);
}


void MainWindow::on_pushButton_18_clicked()
{
    //请求时钟同步

    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1";
        ui->lineEdit->setText(str);
    }

    int station_no = str.toInt();
    app_send_request_sync_time(station_no);
}

void MainWindow::on_pushButton_19_clicked()
{
    //回应时钟同步

    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1";
        QDateTime current = QDateTime::currentDateTime();
        str.append(current.toString(",yyyy,MM,dd,HH,mm,ss"));
        ui->lineEdit->setText(str);
    }

    QStringList list = str.split(",");
    int list_count = 7;
    if(list_count!=list.count())
    {
        return;
    }

    int station_no = list.at(0).toInt();
    jk_time_t time;
    memset(&time, 0, sizeof(time));
    time.year = list.at(1).toInt();
    time.month = list.at(2).toInt();
    time.day = list.at(3).toInt();
    time.hour = list.at(4).toInt();
    time.minute = list.at(5).toInt();
    time.second = list.at(6).toInt();

    app_send_reply_sync_time(station_no, &time);

}

void MainWindow::on_pushButton_11_clicked()
{
    //主站向子站传操作票
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0,1;201=1,0;301=0,1";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    int size = count*sizeof(jk_device_t);
    devices.device = (jk_device_t *)malloc(size);
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, size);

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).split(",").at(0).trimmed().toInt();
        int result = pair.at(1).split(",").at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
        d->state_2 = result;
    }
    app_send_ticket(station_no, &devices);
    free(devices.device);
}

void MainWindow::on_pushButton_10_clicked()
{
    //子站向主站上报遥控闭锁继电器状态
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0,1;201=1,0;301=2,1";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    int size = count*sizeof(jk_device_t);
    devices.device = (jk_device_t *)malloc(size);
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, size);

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).split(",").at(0).trimmed().toInt();
        int result = pair.at(1).split(",").at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
        d->state_2 = result;
    }
    app_send_yk_device_state(station_no, &devices);
    free(devices.device);

}

void MainWindow::on_pushButton_12_clicked()
{
    //请求解锁

    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0;201=1;301=0";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    devices.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, count*sizeof(jk_device_t));

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
    }
    app_send_unlock_device(station_no, &devices);
    free(devices.device);

}

void MainWindow::on_pushButton_13_clicked()
{
    //请求闭锁

    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;101=0;201=1;301=0";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(2>list.count()) return;
    jk_device_list_t devices;
    memset(&devices, 0, sizeof(jk_device_list_t));
    int count = list.count()-1;
    devices.count = count;
    devices.device = (jk_device_t *)malloc(count*sizeof(jk_device_t));
    if(NULL==devices.device)
    {
        return;
    }
    memset(devices.device, 0, count*sizeof(jk_device_t));

    int station_no = list.at(0).trimmed().toInt();
    for(int i=1;i<list.count();i++)
    {
        QString line = list.at(i);
        QStringList pair = line.split("=");
        QString name = pair.at(0).trimmed();
        int state = pair.at(1).trimmed().toInt();

        jk_device_t *d = (jk_device_t *)(devices.device+i-1);
        QByteArray ba = name.toLocal8Bit();
        d->device_len = MIN((int)sizeof(d->device), ba.count());
        memcpy(d->device, ba.data(), d->device_len);
        d->state_1 = state;
    }
    app_send_lock_device(station_no, &devices);
    free(devices.device);

}

void MainWindow::on_pushButton_14_clicked()
{
    //全解锁
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1";
        ui->lineEdit->setText(str);
    }

    int station_no = str.toInt();

    app_send_unlock_device_all(station_no);
}

void MainWindow::on_pushButton_16_clicked()
{
    //全闭锁
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1";
        ui->lineEdit->setText(str);
    }

    int station_no = str.toInt();

    app_send_lock_device_all(station_no);
}

void MainWindow::on_pushButton_15_clicked()
{
    //全解锁汇报
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;1";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(1>list.count()) return;

    int station_no = list.at(0).trimmed().toInt();
    int result = list.at(1).toInt();

    app_send_unlock_device_all_result(station_no, result);

}

void MainWindow::on_pushButton_17_clicked()
{
    //全闭锁汇报
    QString str = ui->lineEdit->text();
    if(str.isEmpty())
    {
        str = "1;2";
        ui->lineEdit->setText(str);
    }
    QStringList list = str.split(";");
    if(1>list.count()) return;

    int station_no = list.at(0).trimmed().toInt();
    int result = list.at(1).toInt();

    app_send_unlock_device_all_result(station_no, result);
}
