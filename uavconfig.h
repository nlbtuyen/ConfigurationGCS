#ifndef UAVCONFIG_H
#define UAVCONFIG_H

#include <QWidget>
#include <QProcess>
#include <QMap>
#include <QSettings>
#include <QAbstractButton>

#include "linkmanager.h"
#include "seriallinkinterface.h"
#include "seriallink.h"
#include "uasinterface.h"
#include "aqpramwidget.h"

class AQParamWidget;

namespace Ui {
class UAVConfig;
}

class UAVConfig : public QWidget
{
    Q_OBJECT

public:
    explicit UAVConfig(QWidget *parent = 0);

    bool aqCanReboot;               // can system accept remote restart command?
    bool saveSettingsToAq(QWidget *parent, bool interactive = true);
    bool useRadioSetupParam;

    ~UAVConfig();

signals:
    void hardwareInfoUpdated(void);

private slots:
    void on_btn_quadx_clicked();

    void on_btn_quadplus_clicked();

    void on_btn_hex6_clicked();

    void on_btn_hexy_clicked();

    void adjustUiForHardware();
    void adjustUiForFirmware(); //update radio type

    void radioType_changed(int idx);//radio change
    void saveAQSettings();
    void uasConnected();

    int calRadioSetting();


private:
    int isSelected = 1;
    QString str = ":images/config/";

    void updateCommonImages();

    //AQSettings
    QRegExp fldnameRx;


protected:

    Ui::UAVConfig *ui;

    AQParamWidget* paramaq;
    UASInterface* uas;
    LinkInterface* connectedLink;


};

#endif // UAVCONFIG_H
