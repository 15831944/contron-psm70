INCLUDEPATH += $$PWD
INCLUDEPATH *= $$PWD/..

HEADERS += \
    $$PWD/GroundServerInterface.h \
    $$PWD/psm.h

LIBS += -L$$PWD -lGroundServer

SOURCES += \
    $$PWD/psm.cpp

#dll_file = .
#CopyDll.input = dll_file
#CopyDll.output = psm.txt
#CopyDll.commands= echo Copy GroundServer.dll... && copy /y $$PWD/GroundServer.dll $(DESTDIR)
#CopyDll.name= copy GroundServer.dll...
#CopyDll.CONFIG+=no_link no_clean tartget_predeps
#QMAKE_EXTRA_COMPILERS += CopyDll
