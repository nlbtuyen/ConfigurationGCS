
INCLUDEPATH += src \


SOURCES +=  src/main.cpp\
            src/mainwindow.cpp \
            src/uavconfig.cpp \
            src/aqpramwidget.cpp \
            src/uasparammanager.cpp \
            src/uas.cpp \
            src/mavlinkprotocol.cpp \
            src/parameterinterface.cpp \
            src/qgc.cpp \
            src/uasmanager.cpp \
            src/linkmanager.cpp \
            src/mavlinkuasfactory.cpp \
            src/seriallink.cpp \
            src/mavlinkdecoder.cpp \
            src/uasinfowidget.cpp \
            src/aq_telemetryView.cpp \
            src/aqlinechartwidget.cpp \
            src/ChartPlot.cpp \
            src/LinechartPlot.cpp \
            src/primaryflightdisplay.cpp \
            src/scrollzoomer.cpp \
            src/scrollbar.cpp \
            src/incrementalplot.cpp \
            src/commconfigurationwindow.cpp \
            src/serialconfigurationwindow.cpp \
            $$PWD/hudwidget.cpp \
    $$PWD/window.cpp


HEADERS  += src/mainwindow.h \
            src/uavconfig.h \
            src/aqpramwidget.h \
            src/uasparammanager.h \
            src/uasinterface.h \
            src/uas.h \
            src/mavlinkprotocol.h \
            src/protocolinterface.h \
            src/parameterinterface.h \
            src/autoquadmav.h \
            src/qgc.h \
            src/uasmanager.h \
            src/linkinterface.h \
            src/linkmanager.h \
            src/mavlinkuasfactory.h \
            src/seriallink.h \
            src/seriallinkinterface.h \
            src/mavlinkdecoder.h \
            src/uasinfowidget.h \
            src/mg.h \
            src/aq_telemetryView.h \
            src/aqlinechartwidget.h \
            src/ChartPlot.h \
            src/LinechartPlot.h \
            src/primaryflightdisplay.h \
            src/scrollzoomer.h \
            src/scrollbar.h \
            src/incrementalplot.h \
            src/commconfigurationwindow.h \
            src/serialconfigurationwindow.h \
            $$PWD/hudwidget.h \
    $$PWD/window.h

FORMS    += src/mainwindow.ui \
            src/uasinfo.ui \
            src/uavconfig.ui \
            src/parameterinterface.ui \
            src/aq_telemetryView.ui \
            src/AQLinechart.ui \
            src/linechart.ui \
            src/configaq.ui \
            src/commsettings.ui \
            src/serialsettings.ui

DISTFILES += \
    $$PWD/main.qml \
    $$PWD/BasicCamera.qml \
    $$PWD/WireframeMaterial.qml \
    $$PWD/Model.qml
