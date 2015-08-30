INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

include($$PWD/../../psmlib/psmlib.pri)

HEADERS += \
    $$PWD/psm_sys.h \
    $$PWD/psm_device.h \
    $$PWD/psm_def.h

SOURCES += \
    $$PWD/psm_sys.cpp \
    $$PWD/psm_device.cpp

