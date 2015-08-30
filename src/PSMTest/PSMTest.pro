TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(../common/common.pri)
include(../psmlib/psmlib.pri)

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    ../psmlib/psmlib.pri \
    ../common/common.pri \
    psmdll/psmdll.pri

#win32{
#    system(cd /d $$PWD && qmake -tp vc -r)
#}

