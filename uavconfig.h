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
    void updateCommonImages();


    bool checkAqConnected(bool interactive = false);

    QString aqBinFolderPath;    // absolute path to AQ supporting utils
    const char *platformExeExt; // OS-specific executables suffix (.exe for Win)


signals:
    void hardwareInfoUpdated(void);

private slots:
    //@Trung: Update Image for tab Mixing & Output
    void on_btn_quadx_clicked();
    void on_btn_quadplus_clicked();
    void on_btn_hex6_clicked();
    void on_btn_hexy_clicked();

    //@Hai: update Param to UI
    QString paramNameGuiToOnboard(QString paraName);
    void loadParametersToUI();
    void createAQParamWidget(UASInterface* uas);
    void setRadioChannelDisplayValue(int channelId, float normalized);

    //@Tuyen: AQ FW Flashing
    void flashFW();
    bool checkProcRunning(bool warn = true);
    bool checkAqSerialConnection(QString port = "");
    void flashFwStart();
    void flashFwDfu();
    void setPortName(QString str);
    void fwTypeChange();
    void selectFWToFlash();
    void loadSettings();
    void setFwType();
    void setupPortList();


private:
    int isSelected = 1;
    QString str = ":images/config/";
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

    QTextEdit* activeProcessStatusWdgt;
    bool fwFlashActive;
    QString LastFilePath;


};

#endif // UAVCONFIG_H
