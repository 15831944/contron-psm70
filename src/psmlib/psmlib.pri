INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..
INCLUDEPATH *= $$PWD/WLLibs
INCLUDEPATH *= $$PWD/CommLibs
INCLUDEPATH *= $$PWD/psmdll

psm_libpath += \
    $$PWD/CommLibs/inc \
    $$PWD/CommLibs/inc/record \
    $$PWD/CommLibs/inc/P7Protocol \
    $$PWD/CommLibs/TicketClientInterface \
    $$PWD/CommLibs/BigDataClientInterface \
    $$PWD/CommLibs/RTDBClientInterface \
    $$PWD/CommLibs/WFServiceClientInterface \
    $$PWD/CommLibs/TicketRelateInterface \
    $$PWD/CommLibs/RTDBYKClientInterface

psm_lib += \
    TicketDLL \
    WFClientDLL \
    RTDBClientDLL \
    BigDataTransClientDll \
    RTDBYKDLL \
    TicketRelateDLL \
    TicketRelateDLLEx

HEADERS += \
    $$PWD/CommLibs/BigDataClientInterface/BigDataTransClientInterface.h \
    $$PWD/CommLibs/inc/P7Protocol/P7ProtocolInsType.h \
    $$PWD/CommLibs/inc/record/DSRecord.h \
    $$PWD/CommLibs/inc/P7DataType.h \
    $$PWD/CommLibs/TicketClientInterface/ChartTicketServDataType.h \
    $$PWD/CommLibs/TicketClientInterface/ExtraDef.h \
    $$PWD/CommLibs/TicketClientInterface/TicketDLLInterface.h \
    $$PWD/CommLibs/WFServiceClientInterface/WFClientDLLDataType.h \
    $$PWD/CommLibs/WFServiceClientInterface/WFClientDLLInterface.h \
    $$PWD/CommLibs/RTDBClientInterface/ExtraDef.h \
    $$PWD/CommLibs/RTDBClientInterface/RTDBBusinessInterface.h \
    $$PWD/CommLibs/RTDBClientInterface/RTDBBusinessInterfaceDatatype.h \
    $$PWD/psmdll/psm_def.h \
    $$PWD/psmdll/psm_device.h \
    $$PWD/psmdll/psm_sys.h \
    $$PWD/psmdll/psm_intf.h \
    $$PWD/psmdll/psm_file.h \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateExDataType.h \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateExInterface.h \
    $$PWD/CommLibs/RTDBYKClientInterface/RTDBYKDLLDataType.h \
    $$PWD/CommLibs/RTDBYKClientInterface/RTDBYKDLLInterface.h \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateDataType.h \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateInterface.h

SOURCES += \
    $$PWD/CommLibs/inc/record/DSRecord.cpp \
    $$PWD/psmdll/psm_device.cpp \
    $$PWD/psmdll/psm_sys.cpp \
    $$PWD/psmdll/psm_intf.cpp \
    $$PWD/psmdll/psm_file.cpp

lib_path = $$OUT_PWD

CONFIG(debug, debug|release) {
    lib_path = $$lib_path/debug
} else {
    lib_path = $$lib_path/release
}
win32{
    COPY = copy /y
    MKDIR = mkdir

copy_file += \
    $$PWD/CommLibs/TicketClientInterface/TicketDLL.dll \
    $$PWD/CommLibs/WFServiceClientInterface/WFClientDLL.dll \
    $$PWD/CommLibs/RTDBClientInterface/RTDBClientDLL.dll \
    $$PWD/CommLibs/RTDBClientInterface/libzmq.dll \
    $$PWD/CommLibs/BigDataClientInterface/BigDataTransClientDll.dll \
    $$PWD/CommLibs/RTDBYKClientInterface/RTDBYKDLL.dll \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateDLLEx.dll \
    $$PWD/CommLibs/TicketRelateInterface/TicketRelateDLL.dll
}else{
    COPY = cp
    MKDIR = mkdir -p
}
#只在shadow build下拷贝DLL
!equals(_PRO_FILE_PWD_, OUT_PWD) {
    for(f, copy_file){
        source_file = $$f
        file_name = $$basename(source_file)
        dist_file = $$lib_path/$$file_name
        dist = $$dirname(dist_file)
        win32:dist = $$replace(dist, /, \\)
        win32:source_file = $$replace(source_file, /, \\)
        !exists($$dist):system($$MKDIR $$dist)
        !exists($$dist_file):system($$COPY $$source_file $$dist)
    }
}

for(path, psm_libpath) {
    INCLUDEPATH *= $$path
    LIBS += -L$$path
}

for(lib, psm_lib) {
    LIBS += -l$$lib
}

