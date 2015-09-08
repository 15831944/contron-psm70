TEMPLATE = subdirs

SUBDIRS += \
#    GroundServer \
#    StationServer \
#    StationClient \
#    PSMTest \
#    StationLibrary \
#    IniTest \
#    StationClientDemo \
    MasterSwitcher \
    TcpTester

#StationClientDemo.depends = StationLibrary
#StationClient.depends = StationLibrary

MasterSwitcher.file = MasterSwitcher/src/MasterSwitcher.pro

