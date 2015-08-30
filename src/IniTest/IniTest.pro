#-------------------------------------------------
#
# Project created by QtCreator 2015-06-18T11:46:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = IniTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


include(../common/common.pri)

SOURCES += main.cpp \
    GCInitFile.cpp \
    JKProtocolConfig.cpp

HEADERS += \
    DataType.h \
    GCInitFile.h \
    JKProtocolConfig.h
