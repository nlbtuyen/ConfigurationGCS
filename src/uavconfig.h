#ifndef UAVCONFIG_H
#define UAVCONFIG_H

#include <QWidget>
#include <QProcess>
#include <QMap>
#include <QSettings>
#include <QAbstractButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QComboBox>

#include "linkmanager.h"
#include "seriallinkinterface.h"
#include "seriallink.h"
#include "uasinterface.h"
#include "aqpramwidget.h"
#include "uasmanager.h"
#include "aq_telemetryView.h"

#include "drone.h"

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QtQml>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QOpenGLContext>
#include <QGraphicsObject>
#include <QTimer>

class AQParamWidget;

namespace Ui {
class UAVConfig;
}

class UAVConfig : public QWidget
{
    Q_OBJECT

public:
    explicit UAVConfig(QWidget *parent = 0);
    ~UAVConfig();

    bool checkAqConnected(bool interactive = false);

    QString aqBinFolderPath;    // absolute path to AQ supporting utils
    const char *platformExeExt; // OS-specific executables suffix (.exe for Win)

    bool saveSettingsToAq(QWidget *parent, bool interactive = true);
    void indexHide(int i);

signals:
    void hardwareInfoUpdated(void);

    void TabClicked(int i);

private slots:
    //@Hai: update Param to UI
    QString paramNameGuiToOnboard(QString paraName);
    void loadParametersToUI();
    void createAQParamWidget(UASInterface* uas);
    void setRadioChannelDisplayValue(int channelId, float normalized);
    void getGUIPara(QWidget *parent);

    //@Leo: AQ FW Flashing
    void flashFW();
    bool checkProcRunning(bool warn = true);
    bool checkAqSerialConnection(QString port = "");
    void flashFwDfu();
    void selectFWToFlash();
    void loadSettings();
    bool validateRadioSettings(int);
    void saveDialogButtonClicked(QAbstractButton *btn);
    void prtstexit(int stat);
    void prtstdout();
    QString extProcessError(QProcess::ProcessError err);

    //connect between QSlider & QLineEdit
    void setValueLineEdit(QString str);
    void updateTextEdit(int i);

    //RC Config
    void sendRcRefreshFreq();

    //RC Chart
    void pitchCharts();
    void calculateResult1_RC();
    void calculateYLoca();
    void drawCharts();

    //3D Model
    void load3DModel();

    //Radio
    void radioType_changed(int idx);
    void startCalib();

    // @trung: RC Chart TPA
    void TPAChart();
    void drawChartTPA();

public slots:
    void saveAQSetting();
    void loggingConsole(QString str);

    void TabRadio();
    void TabMotor();
    void TabIMU();
    void TabPID();
    void TabChart();
    void TabOSD();
    void TabUpgrade();
    void TabBLHeli();

    void receiveTextMessage(int uasid, int componentid, int severity, QString text);


private:
    QRegExp fldnameRx;          // these regexes are used for matching field names to AQ params
    QRegExp dupeFldnameRx;

    //RC Config
    QTimer delayedSendRCTimer;  // for setting radio channel refresh freq.
    QList<QProgressBar *> allRadioChanProgressBars;


    QList<QComboBox *> allRadioChanCombos;
    quint8 paramSaveType;
    bool restartAfterParamSave;
    bool aqCanReboot;               // can system accept remote restart command?
    bool useRadioSetupParam;

    //@Leo : Upgrade Firmware
    QProcess ps_master;
    QString portName;
    QSettings settings;
    QString fileToFlash;

    QMovie *movie_left ;
    QMovie *movie_right;
    QMovie *movie_up;
    QMovie *movie_down;
    QMovie *movie_up_160;
    QMovie *movie_down_160;
    QMovie *movie_left_160;

    //RC Chart
    int expo_pitch;
    int rate_pitch;
    int rc_rate;
    static const int i_const[];
    static const int x_loca[];
    float y_loca[8];
    float result1[8];

    //@trung: RC Chart TPA
    double TPA;
    int TPA_breakpoint;

protected:
    Ui::UAVConfig *ui;
    AQParamWidget* paramaq;
    UASInterface* uas;
    LinkInterface* connectedLink;
    AQTelemetryView *aqtelemetry;

    QTextEdit* activeProcessStatusWdgt;
    bool fwFlashActive;
    QString LastFilePath;

    void updateButtonView();

    QQuickView view;
    Drone drone;


};

#endif // UAVCONFIG_H
