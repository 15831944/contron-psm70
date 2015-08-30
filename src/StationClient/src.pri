INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..


include($$PWD/../common/common.pri)
include($$PWD/protocol/protocol.pri)
include($$PWD/app/app.pri)
include($$PWD/../psmlib/psmlib.pri)

DEFINES += HAS_PSM

contains(DEFINES, HAS_PSM)
{
#DEFINES += STATIONLIBRARY_LIBRARY
include($$PWD/../StationLibrary/stationdll.pri)
}

HEADERS += \
    $$PWD/station_client.h \
    $$PWD/station_def.h \
    $$PWD/station_server.h

SOURCES += \
    $$PWD/station_client.cpp \
    $$PWD/station_server.cpp

DISTFILES += \
    config.ini

COPYFILES = $$DISTFILES
COPYDIR = $$OUT_PWD
include($$PWD/../common/copy.pri)
