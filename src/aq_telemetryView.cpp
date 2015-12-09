#include "aq_telemetryView.h"
#include "ui_aq_telemetryView.h"
#include "uasmanager.h"
#include <QLineEdit>
#include <QFileDialog>
#include <QScreen>
#include <QDateTime>

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

    // save size of this data set
    totalDatasetFields[dset] = telemDataFields.size();

    //init before connect
    init();
    //init after connect
    initChart(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(initChart(UASInterface*)), Qt::UniqueConnection);    
//    connect(ui->combo_refreshRate, SIGNAL(currentIndexChanged(int)), this, SLOT(chartReset(int)));
//    connect(ui->btn_pass, SIGNAL(clicked()), this, SLOT(btnPassClicked()));
//    connect(ui->btn_fail, SIGNAL(clicked()), this, SLOT(btnFailClicked()));

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

void AQTelemetryView::btnPassClicked()
{
    takeScreenshot("pass");
}

void AQTelemetryView::btnFailClicked()
{
    takeScreenshot("fail");
}

void AQTelemetryView::takeScreenshot(QString btnName)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if(screen)
        originalPixmap = screen->grabWindow(QApplication::activeWindow()->winId(), 69, 59, 733, 402);
    QString format = "png";
    QDateTime currentDate = QDateTime::currentDateTime();
    QTime currentTime = QTime::currentTime();
    QString filename = btnName + "_" + currentDate.toString("ddMMyyyy")
            + "_" + currentTime.toString("hhmmss");
    QString filePath = QDir::currentPath() + "/" + filename + "." + format;
    qDebug() << "Save to " + filePath; //@trung
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), initialPath,
//                                                   tr("%1 Files (*.%2);;All Files (*)")
//                                                   .arg(format.toUpper())
//                                                   .arg(format));
//    qDebug() << fileName;
//    if (!fileName.isEmpty())
    originalPixmap.save(filePath, format.toLatin1().constData());
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

void AQTelemetryView::chartReset(int f){
    Q_UNUSED(f);
    if (!uas)
        return;

    // stop telemetry
    disconnect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
//    float freq = ui->combo_refreshRate->itemData(currentRefreshRate).toFloat();

    // start telemetry
    connect(uas, SIGNAL(TelemetryChangedF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)), this, SLOT(getNewTelemetryF(int,mavlink_aq_telemetry_f_t,mavlink_attitude_t)));
//    uas->startStopTelemetry(true, freq, 0);
}

void AQTelemetryView::getNewTelemetry(int uasId, int valIdx){
    float val;
    msec = 0;

    // check selected curve list
    if (ui->combo_selectCurve->currentIndex() != currentCurvedList){
        currentCurvedList = ui->combo_selectCurve->currentIndex();
    }

//    qDebug() << "ok getnew";
    for (int i=0; i < telemDataFields.size(); i++) {
//        qDebug() << "valIdx=" << valIdx << ", telemDataFields[" << i << "].msgValueIndex=" << telemDataFields[i].msgValueIndex;
        if (valIdx == telemDataFields[i].msgValueIndex) {
            val = getTelemValue(telemDataFields[i].valueIndex);
            if (ui->plotFrameTele->isVisible()) {
//                qDebug() << "i=" << i;
                if (currentCurvedList == 0 && i < 3){
//                    qDebug() << "ok";
                    QVariant var = QVariant::fromValue(val);
                    AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, msec);
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, true);
                }else if (currentCurvedList == 1 && i >= 3 && i < 6){
//                    qDebug() << "ok1";
                    QVariant var = QVariant::fromValue(val);
                    AqTeleChart->appendData(uasId, telemDataFields[i].label, "", var, msec);
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, true);
                }
                // Set visible curve
                if (currentCurvedList == 1 && i < 3){
//                    qDebug() << "ok2";
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, false);
                }else if (currentCurvedList == 0 && i >= 3 && i < 6){
//                    qDebug() << "ok3";
                    AqTeleChart->setCurveVisible(telemDataFields[i].label, false);
                }
            }            
        }
    }
}

void AQTelemetryView::getNewTelemetryF(int uasId, mavlink_aq_telemetry_f_t values, mavlink_attitude_t value){
//    qDebug() << "ok emit";
    currentValuesF = &values;
    testValue = &value;
    currentValueType = TELEM_VALUETYPE_FLOAT;
    getNewTelemetry(uasId, 0);
}

