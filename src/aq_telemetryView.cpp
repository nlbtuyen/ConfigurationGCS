#include "aq_telemetryView.h"
#include "ui_aq_telemetryView.h"
#include "uasmanager.h"
#include <QLineEdit>

using namespace AUTOQUADMAV;

AQTelemetryView::AQTelemetryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AQTelemetryView),
    datasetFieldsSetup(-1),
    telemetryRunning(false),
    currentDataSet(TELEM_DATASET_DEFAULT),
    AqTeleChart(NULL),
    uas(NULL)
{

    ui->setupUi(this);

    ui->Frequenz_Telemetry->addItem("25 Hz", 50000);

    // define all data fields

    //valueCallback callback = &AQTelemetryView::getTelemValue;
    QString unit = "float";
    telemDatasets dset = TELEM_DATASET_DEFAULT;
    int msgidx = AQMAV_DATASET_LEGACY1;

    telemDataFields.append(telemFieldsMeta("PITCH", unit, 1, msgidx, dset)); //@trung
    telemDataFields.append(telemFieldsMeta("ROLL", unit, 2, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("YAW", unit, 3, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("PITCH RATE", unit, 4, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("RATE RATE", unit, 5, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("YAW RATE", unit, 6, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("MOTOR A", unit, 7, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("MOTOR B", unit, 8, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("MOTOR C", unit, 9, msgidx, dset));
    telemDataFields.append(telemFieldsMeta("MOTOR D", unit, 10, msgidx, dset));

    // save size of this data set
    totalDatasetFields[dset] = telemDataFields.size();

//    for (int i=0; i < telemDataFields.size(); i++)
//        qDebug() << "Label: " << telemDataFields[i].label << "msgidx: " << telemDataFields[i].msgValueIndex << "dset: " << telemDataFields[i].dataSet;

    setupDataFields();

    connect(ui->pushButton_start_tel_grid, SIGNAL(clicked()),this, SLOT(teleValuesToggle()));
    connect(ui->Frequenz_Telemetry, SIGNAL(activated(int)),this, SLOT(frequencyChanged(int)));

    initChart(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(initChart(UASInterface*)), Qt::UniqueConnection);
}

AQTelemetryView::~AQTelemetryView()
{
    delete ui;
}

void AQTelemetryView::setupDataFields() {
    if (datasetFieldsSetup != currentDataSet) {

        const int totalFlds = totalDatasetFields[currentDataSet];
        const int rowsPerCol = (int) ceil((float)(totalFlds / 4.0f));
        int gridRow = 0;
        int curGrid = 0;
        int fldCnt = 0;
        int i;
        QGridLayout *grid;

        // clear data fields grid
        QLayoutItem* item;
        for (i=0; i < 3; i++) {
            grid = ui->valuesGrid->findChild<QGridLayout *>(QString("valsGrid%1").arg(i));
            while ( ( item = grid->layout()->takeAt(0) ) != NULL ) {
                if (item->widget())
                    delete item->widget();
                delete item;
            }
        }

        grid = ui->valsGrid0;
        for (i=0; i < telemDataFields.size(); i++) {
            if (telemDataFields[i].dataSet == currentDataSet) {
                if ( gridRow >= rowsPerCol && curGrid < 3 ) {
                    curGrid++;
                    grid = ui->valuesGrid->findChild<QGridLayout *>(QString("valsGrid%1").arg(curGrid));
                    gridRow = 0;
                }

                grid->addWidget(new QLabel(telemDataFields[i].label), gridRow, 0);
                grid->addWidget(new QLineEdit, gridRow, 1);

                gridRow = grid->rowCount();
                fldCnt++;
            } // if field is in correct data set
        } // loop over all fields

        datasetFieldsSetup = currentDataSet;
    }
}

void AQTelemetryView::setupCurves() {

    if (!uas) return;

    int uasId = uas->getUASID();
//    qDebug() << uasId;

    // AqTeleChart->clearCurves();

    // populate curves
    for (int i=0; i < telemDataFields.size(); i++) {
        if (telemDataFields[i].dataSet == currentDataSet) {
            QVariant var = QVariant::fromValue(0.0f);
            AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, 0);
        }
    }

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
    float ret = 0.0f, x, y, z;
    switch(idx) {
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
//        ret = currentValuesF->value7;
        break;
    case 8 :
//        ret = currentValuesF->value8;
        break;
    case 9 :
//        ret = currentValuesF->value9;
        break;
    case 10 :
//        ret = currentValuesF->value10;
        break;
    case 1 : //@trung
        ret = testValue->pitch;
        break;
    case 2 :
        ret = testValue->roll;
        break;
    case 3 :
        ret = testValue->yaw;
        break;
    }
    return ret;
}

void AQTelemetryView::teleValuesStart(){

    if (!uas) return;
    connect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
}

void AQTelemetryView::teleValuesStop() {
    if (!uas) return;
    disconnect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
}

void AQTelemetryView::teleValuesToggle(){
    if (!telemetryRunning) {
        teleValuesStart();
        telemetryRunning = true;
        ui->pushButton_start_tel_grid->setText("Stop Telemetry");
    } else {
        teleValuesStop();
        telemetryRunning = false;
        ui->pushButton_start_tel_grid->setText("Start Telemetry");
    }
}

void AQTelemetryView::frequencyChanged(int freq) {
    Q_UNUSED(freq);
    if (telemetryRunning) {
        teleValuesStop();
        teleValuesStart();
    }
}

void AQTelemetryView::getNewTelemetry(int uasId, int valIdx){
    float val;
    QString valStr;
    msec = 0;

    for (int i=0; i < telemDataFields.size(); i++) {
        if (valIdx == telemDataFields[i].msgValueIndex) {
            val = getTelemValue(telemDataFields[i].valueIndex);

            if (ui->tab_val_grid->isVisible()) {
                valStr = telemDataFields[i].unit == QString("int") ? QString::number((int)val) : QString::number(val);
                ui->valuesGrid->findChildren<QLineEdit *>(QString("")).at(i)->setText(valStr);
            }

            if (ui->tab_val_chart->isVisible()) { // AqTeleChart->CurveIsActive[i]
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

    getNewTelemetry(uasId, values.Index);
}
