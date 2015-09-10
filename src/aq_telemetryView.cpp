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

    isRunning = false;
    listChanged = false;
    currentRefreshRate = 2;
    // add content combobox refresh rate
//    ui->combo_refreshRate->addItem("1 ms", 1000000);
//    ui->combo_refreshRate->addItem("50 ms", 100000); //20
//    ui->combo_refreshRate->addItem("100 ms", 50000); //10
//    ui->combo_refreshRate->addItem("200 ms", 20000); //5
//    ui->combo_refreshRate->setCurrentIndex(2);

    ui->combo_selectCurve->addItem("Pitch, Roll, Yaw");
    ui->combo_selectCurve->addItem("Pitch Rate, Roll Rate, Yaw Rate");
    ui->combo_selectCurve->setCurrentIndex(0);
    currentCurvedList = ui->combo_selectCurve->currentIndex();

    // define all data fields
    QString unit = "float";
    telemDatasets dset = TELEM_DATASET_DEFAULT;
    int msgidx = AQMAV_DATASET_LEGACY1;

    telemDataFields.append(telemFieldsMeta("Pitch", unit, 1, msgidx, dset)); // @trung
    telemDataFields.append(telemFieldsMeta("Roll", unit, 2, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Yaw", unit, 3, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Pitch Rate", unit, 4, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Roll Rate", unit, 5, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("Yaw Rate", unit, 6, msgidx, dset));
//    telemDataFields.append(telemFieldsMeta("Motor A", unit, 7, msgidx, dset));
//    telemDataFields.append(telemFieldsMeta("Motor B", unit, 8, msgidx, dset));
//    telemDataFields.append(telemFieldsMeta("Motor C", unit, 9, msgidx, dset));
//    telemDataFields.append(telemFieldsMeta("Motor D", unit, 10, msgidx, dset));

    // save size of this data set
    totalDatasetFields[dset] = telemDataFields.size();
//    connect(ui->combo_refreshRate, SIGNAL(activated(int)),this, SLOT(chartReset(int)));
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
            AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, 0);//, isRunning, i+1, listChanged);
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
    case 1 : // @trung
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
//    case 7 :
//        //Motor A
//        break;
//    case 8 :
//        //Motor B
//        break;
//    case 9 :
//        //Motor C
//        break;
//    case 10 :
//        //Motor D
//        break;
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
            AqTeleChart->appendData(0, telemDataFields[i].label, "", var, 0);//, isRunning, i, listChanged);
        }
    }
}

void AQTelemetryView::chartReset(int f){
//    Q_UNUSED(f);
//    if (!uas)
//        return;

//    // stop telemetry
//    disconnect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
//    float freq = ui->combo_refreshRate->itemData(currentRefreshRate).toFloat();

//    // start telemetry
//    connect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
//    uas->startStopTelemetry(true, freq, 0);
}

void AQTelemetryView::getNewTelemetry(int uasId, int valIdx){
    float val;
    msec = 0;
//    isRunning = true;

    // check refresh rate
//    if (ui->combo_refreshRate->currentIndex() != currentRefreshRate){
//        currentRefreshRate = ui->combo_refreshRate->currentIndex();
//        chartReset(currentRefreshRate);
//    }

    // check selected curve list
    if (ui->combo_selectCurve->currentIndex() != currentCurvedList){
        currentCurvedList = ui->combo_selectCurve->currentIndex();
        listChanged = true;
    }


    for (int i=0; i < telemDataFields.size(); i++) {
        if (valIdx == telemDataFields[i].msgValueIndex) {
            val = getTelemValue(telemDataFields[i].valueIndex);
            if (ui->plotFrameTele->isVisible()) {
                if (currentCurvedList == 0 && i < 3){
                    QVariant var = QVariant::fromValue(val);
                    AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, msec);//, isRunning, i, listChanged);
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, true);
//                    if (i == 2) listChanged = false;
                }else if (currentCurvedList == 1 && i >= 3 && i < 6){
                    QVariant var = QVariant::fromValue(val);
                    AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, msec);//, isRunning, i, listChanged);
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, true);
//                    if (i == 5) listChanged = false;
                }
                // Set visible curve
                if (currentCurvedList == 1 && i < 3){
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, false);
                }else if (currentCurvedList == 0 && i >= 3 && i < 6){
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, false);
                }
            }            
        }
    }
}

void AQTelemetryView::getNewTelemetryF(int uasId, mavlink_aq_telemetry_f_t values, mavlink_attitude_t value){
    currentValuesF = &values;
    testValue = &value;
    currentValueType = TELEM_VALUETYPE_FLOAT;
    getNewTelemetry(uasId, values.Index);
}

