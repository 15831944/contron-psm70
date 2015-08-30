TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt



SOURCES += \
    main.cpp

include(../tests.pri)
include($$common_dir/sys/sys.pri)


include(deployment.pri)
qtcAddDeployment()

