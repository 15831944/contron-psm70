INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

HEADERS += \
    $$PWD/switchertestapp.h \
    $$PWD/log.h \
    $$PWD/config.h \
    $$PWD/testserver.h \
    $$PWD/testclient.h \
    $$PWD/tester.h \
    $$PWD/heartbeat.h \
    $$PWD/heartbeatprotocol.h

SOURCES += \
    $$PWD/switchertestapp.cpp \
    $$PWD/log.cpp \
    $$PWD/config.cpp \
    $$PWD/testserver.cpp \
    $$PWD/testclient.cpp \
    $$PWD/tester.cpp \
    $$PWD/heartbeat.cpp \
    $$PWD/heartbeatprotocol.cpp

