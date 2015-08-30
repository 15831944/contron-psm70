#-------------------------------------------------
#
# Project created by QtCreator 2015-04-14T11:13:51
#
#-------------------------------------------------

#QT       -= core gui
QT       += core
QT       -= gui
#CONFIG   -= qt


TARGET = StationLibrary
TEMPLATE = lib

DEFINES += STATIONLIBRARY_LIBRARY

include($$PWD/../common/common.pri)
include(stationdll.pri)

!isEmpty(DLL_PATH) {
    DESTDIR=$$DLL_PATH
}
!isEmpty(DLL_TARGET) {
    TARGET=$$DLL_TARGET
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

