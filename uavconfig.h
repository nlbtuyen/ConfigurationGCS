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

public slots:
    void loadParametersToUI();
    void createAQParamWidget(UASInterface* uas);

public:
    QString paramNameGuiToOnboard(QString paraName);


private:
    Ui::UAVConfig *ui;
    int isSelected = 1;
    QString str = ":images/config/";
    QRegExp fldnameRx;          // these regexes are used for matching field names to AQ params
    QRegExp dupeFldnameRx;

    void updateCommonImages();

protected:
    AQParamWidget* paramaq;
    UASInterface* uas;


};

#endif // UAVCONFIG_H
