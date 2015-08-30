TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

OUT_PWD =

include($$PWD/../base/common.pri)
include(app/app.pri)

SOURCES += \
    main.cpp \

!win32 {
    QMAKE_LFLAGS += -lpthread
}

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

include(deployment.pri)
qtcAddDeployment()

