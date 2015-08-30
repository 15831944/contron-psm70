TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(../tests.pri)
include($$common_dir/sys/sys.pri)
include($$common_dir/conn/conn.pri)

#LIBS += -lWs2_32

include(deployment.pri)
qtcAddDeployment()

