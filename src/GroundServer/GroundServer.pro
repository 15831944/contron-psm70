TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#接入PSM系统要启用下面的DEFINES
DEFINES += HAS_PSM

include(app/app.pri)
#include(sys/sys.pri)
#include(conn/conn.pri)
include(../common/common.pri)
contains(DEFINES, HAS_PSM) {
    include(psm/psm.pri)
}

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    common/common.pri \
    GroundController/GroundController.pri \
    sys/sys.pri \
    conn/conn.pri \
    psm/psm.pri

