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

private slots:
    //@Hai: update Param to UI
    QString paramNameGuiToOnboard(QString paraName);
    void loadParametersToUI();
    void createAQParamWidget(UASInterface* uas);
    void setRadioChannelDisplayValue(int channelId, float normalized);
    void getGUIPara(QWidget *parent);

    //@Tuyen: AQ FW Flashing
    void flashFW();
    bool checkProcRunning(bool warn = true);
    bool checkAqSerialConnection(QString port = "");
    void flashFwDfu();
    void selectFWToFlash();
    void loadSettings();
    void saveAQSetting();
    bool validateRadioSettings(int);
    void saveDialogButtonClicked(QAbstractButton *btn);

    void prtstexit(int stat);
    void prtstdout();
    QString extProcessError(QProcess::ProcessError err);

    void on_RADIOpushButton_clicked();

    void on_MOTORpushButton_clicked();

    void on_IMUpushButton_clicked();

    void on_PIDTURNINGpushButton_clicked();

    void on_CHARTpushButton_clicked();

    void on_UPGRADEpushButton_clicked();

    void on_OSDpushButton_clicked();

private:
    QRegExp fldnameRx;          // these regexes are used for matching field names to AQ params
    QRegExp dupeFldnameRx;

    QList<QComboBox *> allRadioChanCombos;
    QList<QProgressBar *> allRadioChanProgressBars;
    quint8 paramSaveType;
    bool restartAfterParamSave;
    bool aqCanReboot;               // can system accept remote restart command?
    bool useRadioSetupParam;


    //@Tuyen
    QProcess ps_master;
    QString portName;
    QSettings settings;
    QString fileToFlash;



protected:
    Ui::UAVConfig *ui;
    AQParamWidget* paramaq;
    UASInterface* uas;
    LinkInterface* connectedLink;
    AQTelemetryView *aqtelemetry;

    QTextEdit* activeProcessStatusWdgt;
    bool fwFlashActive;
    QString LastFilePath;


};

#endif // UAVCONFIG_H
