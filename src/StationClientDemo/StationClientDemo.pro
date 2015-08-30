#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T14:11:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StationClientDemo
TEMPLATE = app

#DEFINES += STATIONLIBRARY_LIBRARY

include($$PWD/../common/common.pri)

!contains(DEFINES, STATIONLIBRARY_LIBRARY) {
INCLUDEPATH += $$PWD/../psmlib/psmdll/
HEADERS += \
    $$PWD/../psmlib/psmdll/psm_def.h
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#DEFINES += STATIONLIBRARY_LIBRARY

include($$PWD/../StationLibrary/stationdll.pri)

#use in vs
#copy /y $(ProjectDir)..\StationLibrary\lib\StationLibrary.dll $(OutDir)

