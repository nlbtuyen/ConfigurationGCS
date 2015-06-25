#-------------------------------------------------
#
# Project created by QtCreator 2015-05-16T09:52:19
#
#-------------------------------------------------
TARGET = SerialConnectorMavlink
TEMPLATE = app


# root of project files
BASEDIR = $$_PRO_FILE_PWD_
# build directories
DESTDIR = $${OUT_PWD}
BUILDDIR = $${OUT_PWD}/build
OBJECTS_DIR = $${BUILDDIR}/obj
MOC_DIR = $${BUILDDIR}/moc
UI_DIR = $${BUILDDIR}/ui
RCC_DIR = $${BUILDDIR}/rcc
MOC_DIR = $${BUILDDIR}/moc

QT       += core gui widgets

CONFIG(release, debug|release) {
        message(Release build)
        CONFIG += ReleaseBuild
}

CONFIG += extserialport static qesp_static
CONFIG += WinBuild

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$BASEDIR/libs/lib/sdl/msvc/include \
                $$BASEDIR/libs/lib/sdl/include

# Include QWT plotting library
include(libs/qwt/qwt.pri)

include(install.pri)

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
    qextserialenumerator_win.cpp \
    aq_telemetryView.cpp \
    aqlinechartwidget.cpp \
    ChartPlot.cpp \
    LinechartPlot.cpp \
    primaryflightdisplay.cpp \
    hddisplay.cpp


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
    qextserialenumerator_p.h \
    aq_telemetryView.h \
    aqlinechartwidget.h \
    ChartPlot.h \
    LinechartPlot.h \
    primaryflightdisplay.h \
    hddisplay.h


FORMS    += mainwindow.ui \
    serialsettings.ui \
    uasinfo.ui \
    uavconfig.ui \
    parameterinterface.ui \
    mavlinkmessagesender.ui \
    commsettings.ui \
    debugconsole.ui \
    aq_telemetryView.ui \
    AQLinechart.ui \
    hddisplay.ui



RESOURCES += \
    terminal.qrc



DEFINES *= QT_USE_QSTRINGBUILDER

OTHER_FILES += styles/*.css


