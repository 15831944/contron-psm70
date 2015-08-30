INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

HEADERS += \
    $$PWD/jk_crc.h \
    $$PWD/jk_function.h \
    $$PWD/jk_def.h \
    $$PWD/jk_core.h \
    $$PWD/jk_worker.h \
    $$PWD/jk_worker_protocol.h \
    $$PWD/jk_worker_common.h \
    $$PWD/jk_intf.h \
    $$PWD/jk_cmd_buffer.h

SOURCES += \
    $$PWD/jk_crc.cpp \
    $$PWD/jk_function.cpp \
    $$PWD/jk_core.cpp \
    $$PWD/jk_worker.cpp \
    $$PWD/jk_worker_client.cpp \
    $$PWD/jk_worker_protocol.cpp \
    $$PWD/jk_worker_server.cpp \
    $$PWD/jk_worker_common.cpp \
    $$PWD/jk_intf.cpp \
    $$PWD/jk_cmd_buffer.cpp
