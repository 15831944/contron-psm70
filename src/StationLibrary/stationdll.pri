INCLUDEPATH += $$PWD

DLL_PATH = $$PWD/lib
DLL_TARGET = StationLibrary

!contains(DEFINES, STATIONLIBRARY_LIBRARY) {
LIBS += -L$$DLL_PATH -l$$DLL_TARGET
#win32 {
#DLL_FILE = $${DLL_PATH}/$${DLL_TARGET}.dll
#COPYFILES += $$DLL_FILE
#}

#file_path = $$OUT_PWD
#CONFIG(debug, debug|release) {
#    file_path = $$file_path/debug
#} else {
#    file_path = $$file_path/release
#}
#COPYDIR = $$file_path

#include($$PWD/../common/copy.pri)
}

contains(DEFINES, STATIONLIBRARY_LIBRARY) {
INCLUDEPATH += $$PWD/../psmlib/psmdll/
HEADERS += \
    $$PWD/../psmlib/psmdll/psm_def.h
SOURCES += $$PWD/stationlibrary.cpp
}

HEADERS += $$PWD/stationlibrary.h\
        $$PWD/stationlibrary_global.h

file_path = $$OUT_PWD

CONFIG(debug, debug|release) {
    file_path = $$file_path/debug
} else {
    file_path = $$file_path/release
}
win32{
    COPY = copy /y
    MKDIR = mkdir
}else{
    COPY = cp
    MKDIR = mkdir -p
}
copy_file =
win32 {
copy_file += $$DLL_PATH/$$DLL_TARGET.dll
}
#只在shadow build下拷贝DLL
!equals(_PRO_FILE_PWD_, OUT_PWD) {
    for(f, copy_file){
        source_file = $$f
        file_name = $$basename(source_file)
        dist_file = $$file_path/$$file_name
        dist = $$dirname(dist_file)
        win32:dist = $$replace(dist, /, \\)
        win32:source_file = $$replace(source_file, /, \\)
        !exists($$dist):system($$MKDIR $$dist)
        system($$COPY $$source_file $$dist)
    }
}
