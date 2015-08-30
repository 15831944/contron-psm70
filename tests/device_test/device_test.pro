#-------------------------------------------------
#
# Project created by QtCreator 2015-05-27T17:17:54
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_device_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../src/psmlib/psmlib.pri)
include(../../src/common/common.pri)

SOURCES += tst_device_testtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DISTFILES += \
    ../../src/psmlib/psmdll/psmdll.pri
