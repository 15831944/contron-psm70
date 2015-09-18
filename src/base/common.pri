INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

include(sys/sys.pri)
include(conn/conn.pri)
include(utils/utils.pri)

DISTFILES += \
    $$PWD/readme.txt
