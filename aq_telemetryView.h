#ifndef AQ_TELEMETRYVIEW_H
#define AQ_TELEMETRYVIEW_H

#include "aqlinechartwidget.h"
#include "autoquadMAV.h"
#include <QTabWidget>

namespace Ui {
class AQTelemetryView;
}

using namespace AUTOQUADMAV;

class AQTelemetryView : public QWidget
{
    Q_OBJECT

public:
    explicit AQTelemetryView(QWidget *parent = 0);
    ~AQTelemetryView();

private:
    enum telemDatasets { TELEM_DATASET_DEFAULT, TELEM_DATASET_GROUND, TELEM_DATASET_NUM };
    enum telemValueTypes { TELEM_VALUETYPE_FLOAT, TELEM_VALUETYPE_INT };
    enum telemValueDefs { TELEM_VALDEF_ACC_MAGNITUDE = 100, TELEM_VALDEF_MAG_MAGNITUDE, TELEM_VALDEF_ACC_PITCH, TELEM_VALDEF_ACC_ROLL };

    struct telemFieldsMeta {
        telemFieldsMeta(QString label, QString unit, int valueIndex, int msgValueIndex = 0, telemDatasets dataSet = TELEM_DATASET_DEFAULT) :
            label(label), unit(unit), valueIndex(valueIndex), msgValueIndex(msgValueIndex), dataSet(dataSet) {}

        QString label; // human-readable name of field
        QString unit; // value type (float|int)
        int valueIndex; // index of telemtry value in mavlink msg
        int msgValueIndex; // __mavlink_aq_telemetry_[f|i]_t.Index
        telemDatasets dataSet;
    };

    Ui::AQTelemetryView *ui;
    int msec;
    int totalDatasetFields[TELEM_DATASET_NUM];
    int datasetFieldsSetup;
    bool telemetryRunning;
    QGridLayout* linLayoutPlot;
    mavlink_aq_telemetry_f_t *currentValuesF;
    telemValueTypes currentValueType;
    telemDatasets currentDataSet;
    QList<telemFieldsMeta> telemDataFields;
    QButtonGroup* btnsDataSets;

    void setupDataFields();
    void setupCurves();
    float getTelemValue(const int idx);

public slots:
    void initChart(UASInterface *uav);


private slots:
    void getNewTelemetry(int uasId, int valIdx);
    void getNewTelemetryF(int uasId, mavlink_aq_telemetry_f_t values);

    void teleValuesToggle();
    void teleValuesStart();
    void teleValuesStop();
    void frequencyChanged(int freq);

protected:
    AQLinechartWidget* AqTeleChart;
    UASInterface* uas;
};

#endif // AQ_TELEMETRYVIEW_H
