INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

HEADERS += \
    $$PWD/tcp.h

SOURCES += \
    $$PWD/tcp.cpp

win32 {
    LIBS += -lWs2_32
}
