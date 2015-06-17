#-------------------------------------------------
#
# Project created by QtCreator 2015-05-16T09:52:19
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += extserialport static qesp_static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialConnectorMavlink
TEMPLATE = app

BASEDIR = $$_PRO_FILE_PWD_
MOC_DIR = $${BUILDDIR}/moc

INCLUDEPATH += $$BASEDIR/libs/lib/sdl/msvc/include \
                $$BASEDIR/libs/lib/sdl/include

LIBS += -L$$BASEDIR/libs/lib/sdl/msvc/lib

LIBS +=  -lSDLmain -lSDL -lsetupapi -lsetupapi -ladvapi32 -luser32

SOURCES += main.cpp\
        mainwindow.cpp \
    uavconfig.cpp \
    aqpramwidget.cpp \
    uasparammanager.cpp \
    uas.cpp \
    mavlinkprotocol.cpp \
    parameterinterface.cpp \
    qgc.cpp \
    uasmanager.cpp \
    mavlinkmessagesender.cpp \
    linkmanager.cpp \
    mavlinkuasfactory.cpp \
    seriallink.cpp \
    mavlinkdecoder.cpp \
    uasinfowidget.cpp \
    serialconfigurationwindow.cpp \
    commconfigurationwindow.cpp \
    debugconsole.cpp \
    qextserialport.cpp \
    qextserialenumerator.cpp \
    qextserialport_win.cpp \
    qextserialenumerator_win.cpp


HEADERS  += mainwindow.h \
common\* \
    uavconfig.h \
    aqpramwidget.h \
    uasparammanager.h \
    uasinterface.h \
    uas.h \
    mavlinkprotocol.h \
    protocolinterface.h \
    autoquad\* \
    parameterinterface.h \
    autoquadmav.h \
    qgc.h \
    uasmanager.h \
    eigen\* \
    mavlinkmessagesender.h \
    linkinterface.h \
    linkmanager.h \
    mavlinkuasfactory.h \
    seriallink.h \
    seriallinkinterface.h \
    mavlinkdecoder.h \
    uasinfowidget.h \
    mg.h \
    serialconfigurationwindow.h \
    commconfigurationwindow.h \
    debugconsole.h \
    qextserialport.h \
    qextserialport_global.h \
    qextserialport_p.h \
    qextserialenumerator.h \
    qextserialenumerator_p.h


FORMS    += mainwindow.ui \
    serialsettings.ui \
    uasinfo.ui \
    uavconfig.ui \
    parameterinterface.ui \
    mavlinkmessagesender.ui \
    commsettings.ui \
    debugconsole.ui



RESOURCES += \
    terminal.qrc



DEFINES *= QT_USE_QSTRINGBUILDER
