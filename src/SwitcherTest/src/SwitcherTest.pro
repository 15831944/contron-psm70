TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/../../base/common.pri)
include(app/app.pri)
include(other.pri)

SOURCES += main.cpp


!win32 {
    QMAKE_LFLAGS += -pthread
}

#输出目录
OUT_DIR = $$OUT_PWD/../../../

#程序目录
DESTDIR = $$OUT_DIR/bind

#临时文件目录
OBJECTS_DIR = $$OUT_DIR/out

CONFIG(debug, debug|release) {
    OBJECTS_DIR = $$OBJECTS_DIR/$$TARGET/debug
} else {
    OBJECTS_DIR = $$OBJECTS_DIR/$$TARGET/release
}

#!equals(_PRO_FILE_PWD_, $$OUT_PWD) { #only do something in case of shadow build

#拷贝脚本和配置文件

config_ini.source = test_config.ini
config_ini.target = ../../../ini

log_dir.source = readme.txt
log_dir.target = ../../../data

DEPLOYMENTFOLDERS += config_ini log_dir

include(deployment.pri)
qtcAddDeployment()
