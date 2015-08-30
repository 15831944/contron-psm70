TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

include($$PWD/../../base/common.pri)
include(app/app.pri)

SOURCES += \
    main.cpp \

!win32 {
    QMAKE_LFLAGS += -lpthread
}

#输出目录
OUT_DIR = $$absolute_path($$OUT_PWD/../../../)
OUT_PWD = $$OUT_DIR

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
ping_bat.source = scripts/ping.bat
ping_bat.target = scripts

ping_sh.source = scripts/ping.sh
ping_sh.target = scripts

ip_bat.source = scripts/ip.bat
ip_bat.target = scripts

ip_sh.source = scripts/ip.sh
ip_sh.target = scripts

onmaster_bat.source = scripts/onmaster.bat
onmaster_bat.target = scripts

onmaster_sh.source = scripts/onmaster.sh
onmaster_sh.target = scripts

config_ini.source = config.ini
config_ini.target = ini

DEPLOYMENTFOLDERS += ping_bat ping_sh ip_bat ip_sh onmaster_bat onmaster_sh config_ini

ip_add_bat.source = scripts/ip_add.bat
ip_add_bat.target = scripts
ip_del_bat.source = scripts/ip_del.bat
ip_del_bat.target = scripts
ip_search_bat.source = scripts/ip_search.bat
ip_search_bat.target = scripts

DEPLOYMENTFOLDERS += ip_add_bat ip_del_bat ip_search_bat

include(deployment.pri)
qtcAddDeployment()

