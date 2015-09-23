TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

DEFINES += QT

include($$PWD/../../base/common.pri)
include(app/app.pri)

SOURCES += \
    main.cpp \

!win32 {
    QMAKE_LFLAGS += -lpthread
}

#输出目录
OUT_DIR = $$OUT_PWD/../../../
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

copy_dir = ..
unix {
    copy_dir = ../../..
}
#拷贝脚本和配置文件
ping_bat.source = scripts/ping.bat
ping_bat.target = $$copy_dir/scripts

ping_sh.source = scripts/ping.sh
ping_sh.target = $$copy_dir/scripts

ip_bat.source = scripts/ip.bat
ip_bat.target = $$copy_dir/scripts

ip_sh.source = scripts/ip.sh
ip_sh.target = $$copy_dir/scripts

onmaster_bat.source = scripts/onmaster.bat
onmaster_bat.target = $$copy_dir/scripts

onmaster_sh.source = scripts/onmaster.sh
onmaster_sh.target = $$copy_dir/scripts

config_ini.source = config.ini
config_ini.target = $$copy_dir/ini

DEPLOYMENTFOLDERS += ping_bat ping_sh ip_bat ip_sh onmaster_bat onmaster_sh config_ini

ip_add_bat.source = scripts/ip_add.bat
ip_add_bat.target = $$copy_dir/scripts
ip_del_bat.source = scripts/ip_del.bat
ip_del_bat.target = $$copy_dir/scripts
ip_search_bat.source = scripts/ip_search.bat
ip_search_bat.target = $$copy_dir/scripts

DEPLOYMENTFOLDERS += ip_add_bat ip_del_bat ip_search_bat

onslave_bat.source = scripts/onslave.bat
onslave_bat.target = $$copy_dir/scripts

onslave_sh.source = scripts/onslave.sh
onslave_sh.target = $$copy_dir/scripts

DEPLOYMENTFOLDERS += onslave_bat onslave_sh

ip_add_sh.source = scripts/ip_add.sh
ip_add_sh.target = $$copy_dir/scripts
ip_del_sh.source = scripts/ip_del.sh
ip_del_sh.target = $$copy_dir/scripts
ip_search_sh.source = scripts/ip_search.sh
ip_search_sh.target = $$copy_dir/scripts

DEPLOYMENTFOLDERS += ip_add_sh ip_del_sh ip_search_sh

start_sync_sh.source = scripts/start_sync.sh
start_sync_sh.target = $$copy_dir/scripts
sync_sh.source = scripts/sync.sh
sync_sh.target = $$copy_dir/scripts
stop_sync_sh.source = scripts/stop_sync.sh
stop_sync_sh.target = $$copy_dir/scripts

start_sync_bat.source = scripts/start_sync.bat
start_sync_bat.target = $$copy_dir/scripts
sync_bat.source = scripts/sync.bat
sync_bat.target = $$copy_dir/scripts
stop_sync_bat.source = scripts/stop_sync.bat
stop_sync_bat.target = $$copy_dir/scripts

DEPLOYMENTFOLDERS += start_sync_sh sync_sh stop_sync_sh
DEPLOYMENTFOLDERS += start_sync_bat sync_bat stop_sync_bat

log_dir.source = ../../base/readme.txt
log_dir.target = $$copy_dir/log

tmp_dir.source = ../../base/readme.txt
tmp_dir.target = $$copy_dir/tmp

DEPLOYMENTFOLDERS += log_dir tmp_dir

log_bat.source = scripts/log.bat
log_bat.target = $$copy_dir/scripts

log_sh.source = scripts/log.sh
log_sh.target = $$copy_dir/scripts

DEPLOYMENTFOLDERS += log_bat log_sh

include(deployment.pri)
qtcAddDeployment()

