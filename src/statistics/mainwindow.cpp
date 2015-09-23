#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QCursor>

#include "counter.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::analytic(const QString &fileName)
{
    QFile data(fileName);
    //    if (!data.open(QFile::WriteOnly | QFile::Truncate))
    //    {
    //        return;
    //    }
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    ui->textEdit->append(QString("File name: %1").arg(fileName));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    Counter c;
    while (!data.atEnd()) {
        qApp->processEvents();
        QByteArray line = data.readLine();
        c.receive(line);
//        ui->textEdit->append(QString(line));
        //        process_line(line);
    }
    //    QTextStream out(&data);

    //    QString line = out.readAll();
    //    do {
    //        line = out.readLine();
    //    } while (!out.atEnd());
    ui->textEdit->append("----------------------------");
    ui->textEdit->append(QString("Test: %1\t Max=%2").arg(c.count()).arg(c.max()));
    mTestCount += c.count();
    if(c.max()>mMax)
    {
        mMax = c.max();
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_pushButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Test Files"), QDir::currentPath());
    if(directory.isEmpty())
    {
        return;
    }

    mMax = 0;
    mTestCount = 0;

    QDir dir;
    dir.setPath(directory);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
//    std::cout << "     Bytes Filename" << std::endl;
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        analytic(fileInfo.absolutePath()+"/"+fileInfo.fileName());
//        std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
//                                                .arg(fileInfo.fileName()));
//        std::cout << std::endl;
    }
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                     "",
//                                                     tr("LOGS (*.log)"));
//    QFileInfo fi;
//    fi.setFile(fileName);

//    if(!fi.exists())
//    {
//        return;
//    }
//    ui->lineEdit->setText(fileName);
//    analytic(fileName);
    ui->textEdit->append("*******************************");
    ui->textEdit->append("TOTAL:");
    ui->textEdit->append(QString("Test count: %1").arg(mTestCount));
    ui->textEdit->append(QString("Max: %1").arg(mMax));
}
