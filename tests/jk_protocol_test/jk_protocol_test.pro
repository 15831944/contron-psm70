#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T16:36:27
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_jk_protocol_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../src/StationClient/src.pri)


SOURCES += tst_jk_protocol_testtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DISTFILES += \
    ../../src/StationClient/src.pri
