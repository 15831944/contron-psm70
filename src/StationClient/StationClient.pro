TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = jkserver


include(src.pri)

SOURCES += main.cpp


include(deployment.pri)
qtcAddDeployment()

