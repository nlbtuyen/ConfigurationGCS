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
    ~UAVConfig();
    void updateCommonImages();


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

private:
    int isSelected = 1;
    QString str = ":images/config/";
    QRegExp fldnameRx;          // these regexes are used for matching field names to AQ params
    QRegExp dupeFldnameRx;

    QList<QComboBox *> allRadioChanCombos;
    quint8 paramSaveType;
    bool restartAfterParamSave;
    bool aqCanReboot;               // can system accept remote restart command?
    bool useRadioSetupParam;


protected:
    Ui::UAVConfig *ui;
    AQParamWidget* paramaq;
    UASInterface* uas;


};

#endif // UAVCONFIG_H
