#-------------------------------------------------
#
# Project created by QtCreator 2015-09-01T09:10:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT5

TARGET = TcpTester
TEMPLATE = app

include($$PWD/../../src/base/common.pri)
include(other.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    log.cpp \
    remotepc.cpp \
    localpc.cpp

HEADERS  += mainwindow.h \
    log.h \
    remotepc.h \
    localpc.h

FORMS    += mainwindow.ui

unix {
CONFIG(debug, debug|release) {
QMAKE_CFLAGS += -g
}
}

