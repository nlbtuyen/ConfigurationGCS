#-------------------------------------------------
#
# Project created by QtCreator 2015-05-16T09:52:19
#
#-------------------------------------------------

QT       += core gui serialport widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialConnectorMavlink
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
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
    debugconsole.cpp


HEADERS  += mainwindow.h \
    settingsdialog.h \
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
    debugconsole.h


FORMS    += mainwindow.ui \
    settingsdialog.ui \
    serialsettings.ui \
    uasinfo.ui \
    uavconfig.ui \
    parameterinterface.ui \
    mavlinkmessagesender.ui \
    commsettings.ui \
    debugconsole.ui



RESOURCES += \
    terminal.qrc
