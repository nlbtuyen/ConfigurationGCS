#ifndef AQTESTVIEW_H
#define AQTESTVIEW_H

#include <QWidget>
#include "aqlinecharttestwidget.h"
#include "autoquadMAV.h"
#include "uavconfig.h"
#include <QTabWidget>

namespace Ui {
class AQTestView;
}

using namespace AUTOQUADMAV;

class AQTestView : public QWidget
{
    Q_OBJECT

public:
    explicit AQTestView(QWidget *parent = 0);
    ~AQTestView();

    int currentRefreshRate;

private:
    enum telemDatasets { TELEM_DATASET_DEFAULT, TELEM_DATASET_GROUND, TELEM_DATASET_NUM };
    enum telemValueTypes { TELEM_VALUETYPE_FLOAT, TELEM_VALUETYPE_INT };
    enum telemValueDefs { TELEM_VALDEF_ACC_MAGNITUDE = 100, TELEM_VALDEF_MAG_MAGNITUDE, TELEM_VALDEF_ACC_PITCH, TELEM_VALDEF_ACC_ROLL };

    struct telemFieldsMeta {
        telemFieldsMeta(QString label, QString unit, int valueIndex, int msgValueIndex = 0, telemDatasets dataSet = TELEM_DATASET_DEFAULT) :
            label(label), unit(unit), valueIndex(valueIndex), msgValueIndex(msgValueIndex), dataSet(dataSet) {}

        QString label;          // human-readable name of field
        QString unit;           // value type (float|int)
        int valueIndex;         // index of telemtry value in mavlink msg
        int msgValueIndex;      // __mavlink_aq_telemetry_[f|i]_t.Index
        telemDatasets dataSet;
    };

    Ui::AQTestView *ui;

    int msec;
    int totalDatasetFields[TELEM_DATASET_NUM];
    int datasetFieldsSetup;
    QGridLayout* linLayoutPlot;
    mavlink_attitude_t *testValue; // @trung
    mavlink_aq_telemetry_f_t *currentValuesF;
    telemValueTypes currentValueType;
    telemDatasets currentDataSet;
    QList<telemFieldsMeta> telemDataFields;
    QButtonGroup* btnsDataSets;
    void setupCurves();
    float getTelemValue(const int idx);
    void init(); //@Leo
    int currentCurvedList;
    QPixmap originalPixmap;
    void takeScreenshot(QString btnName); //@trung
    bool testPassOrFail;
    bool checkTestMessage();

public slots:
    void initChart(UASInterface *uav);
    void btnStartStopClicked();

private slots:
    void getNewTelemetry(int uasId, int valIdx);
    void getNewTelemetryF(int uasId, mavlink_aq_telemetry_f_t values, mavlink_attitude_t value);
    void chartReset(int);
    void updateID(int id, int component, int severity, QString text);

protected:
    AQLinechartTestWidget* AqTeleChart;
    UASInterface* uas;
};

#endif // AQTESTVIEW_H
