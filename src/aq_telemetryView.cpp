#include "aq_telemetryView.h"
#include "ui_aq_telemetryView.h"
#include "uasmanager.h"
#include <QLineEdit>

using namespace AUTOQUADMAV;

AQTelemetryView::AQTelemetryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AQTelemetryView),
    datasetFieldsSetup(-1),
    currentDataSet(TELEM_DATASET_DEFAULT),
    AqTeleChart(NULL),
    uas(NULL)
{
    ui->setupUi(this);

    // define all data fields
    QString unit = "float";
    telemDatasets dset = TELEM_DATASET_DEFAULT;
    int msgidx = AQMAV_DATASET_LEGACY1;

    telemDataFields.append(telemFieldsMeta("Pitch", unit, 1, msgidx, dset)); //@trung
    telemDataFields.append(telemFieldsMeta("Roll", unit, 2, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Yaw", unit, 3, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Pitch Rate", unit, 4, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Roll Rate", unit, 5, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Yaw Rate", unit, 6, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Motor A", unit, 7, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Motor B", unit, 8, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Motor C", unit, 9, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Motor D", unit, 10, msgidx, dset));

    // save size of this data set
    totalDatasetFields[dset] = telemDataFields.size();

    //init before connect
    init();
    //init after connect
    initChart(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(initChart(UASInterface*)), Qt::UniqueConnection);
}

AQTelemetryView::~AQTelemetryView()
{
    delete ui;
}

void AQTelemetryView::setupCurves() {

    if (!uas) return;

    int uasId = uas->getUASID();

    // view curves
    for (int i=0; i < telemDataFields.size(); i++) {
        if (telemDataFields[i].dataSet == currentDataSet) {
            QVariant var = QVariant::fromValue(0.0f);
            AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, 0);
        }
    }
    connect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
}

void AQTelemetryView::initChart(UASInterface *uav) {
    if (!uav) return;
    uas = uav;
    if ( !AqTeleChart ) {
        AqTeleChart = new AQLinechartWidget(uas->getUASID(), ui->plotFrameTele);
        linLayoutPlot = new QGridLayout( ui->plotFrameTele);
        linLayoutPlot->addWidget(AqTeleChart, 0, Qt::AlignCenter);
    }
    setupCurves();
}

float AQTelemetryView::getTelemValue(const int idx) {
    float ret = 0.0f;
    switch(idx) {
    case 1 : //@trung
        ret = testValue->pitch;
        break;
    case 2 :
        ret = testValue->roll;
        break;
    case 3 :
        ret = testValue->yaw;
        break;
    case 4 :
        ret = testValue->pitchspeed;
        break;
    case 5 :
        ret = testValue->rollspeed;
        break;
    case 6 :
        ret = testValue->yawspeed;
        break;
    case 7 :
        //Motor A
        break;
    case 8 :
        //Motor B
        break;
    case 9 :
        //Motor C
        break;
    case 10 :
        //Motor D
        break;
    }
    return ret;
}

void AQTelemetryView::init()
{
    if ( !AqTeleChart ) {
        AqTeleChart = new AQLinechartWidget(0, ui->plotFrameTele);
        linLayoutPlot = new QGridLayout( ui->plotFrameTele);
        linLayoutPlot->addWidget(AqTeleChart, 0, Qt::AlignCenter);
    }
    for (int i=0; i < telemDataFields.size(); i++) {
        if (telemDataFields[i].dataSet == currentDataSet) {
            QVariant var = QVariant::fromValue(0.0f);
            AqTeleChart->appendData(0, telemDataFields[i].label, "", var, 0);
        }
    }
}

void AQTelemetryView::getNewTelemetry(int uasId, int valIdx){
    float val;
    msec = 0;

    for (int i=0; i < telemDataFields.size(); i++) {
        if (valIdx == telemDataFields[i].msgValueIndex) {
            val = getTelemValue(telemDataFields[i].valueIndex);

            if (ui->tab_val_chart->isVisible()) {
                QVariant var = QVariant::fromValue(val);
                AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, msec);
            }
        }
    }
}

void AQTelemetryView::getNewTelemetryF(int uasId, mavlink_aq_telemetry_f_t values, mavlink_attitude_t value){
    currentValuesF = &values;
    testValue = &value;
    currentValueType = TELEM_VALUETYPE_FLOAT;
//    qDebug() << "run";
    getNewTelemetry(uasId, values.Index);
}
