#include "uasinfowidget.h"
#include "uasmanager.h"
#include "mg.h"

#include <QTimer>
#include <QDir>
#include <cstdlib>
#include <cmath>
#include <QDebug>

UASInfoWidget::UASInfoWidget(QWidget *parent, QString name) : QWidget(parent)
{
    ui.setupUi(this);
    this->name = name;

    activeUAS = NULL;

    //instruments = new QMap<QString, QProgressBar*>();

    // Set default battery type
    //    setBattery(0, LIPOLY, 3);
    startTime = MG::TIME::getGroundTimeNow();
    //    startVoltage = 0.0f;

    //    lastChargeLevel = 0.5f;
    //    lastRemainingTime = 1;

    // Set default values
    /** Set two voltage decimals and zero charge level decimals **/
    this->voltageDecimals = 2;
    this->loadDecimals = 2;

    this->voltage = 0;
    this->chargeLevel = 0;
    this->load = 0;
    this->rssi = 0;
    this->gpsFixType = 0;
    this->gpsEph = -1;
    this->gpsEpv = -1;
    receiveLoss = 0;
    sendLoss = 0;
    changed = true;
    errors = QMap<QString, int>();

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    updateTimer->start(updateInterval);

    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(addUAS(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    this->setVisible(false);
}

UASInfoWidget::~UASInfoWidget()
{

}

void UASInfoWidget::showEvent(QShowEvent* event)
{
    // React only to internal (pre-display)
    // events
    Q_UNUSED(event);
    updateTimer->start(updateInterval);
}

void UASInfoWidget::hideEvent(QHideEvent* event)
{
    // React only to internal (pre-display)
    // events
    Q_UNUSED(event);
    updateTimer->stop();
}

void UASInfoWidget::addUAS(UASInterface* uas)
{
    if (uas != NULL && uas->getUASID() == UASManager::instance()->getActiveUAS()->getUASID()) {
        setActiveUAS(uas);
    }
}

void UASInfoWidget::removeUAS(UASInterface *uas)
{
    disconnect(uas, 0, this, 0);
    if (activeUAS && activeUAS->getUASID() == uas->getUASID())
        activeUAS = NULL;
}

void UASInfoWidget::setActiveUAS(UASInterface* uas)
{
    if (uas != NULL && (!activeUAS || activeUAS->getUASID() != uas->getUASID())) {
        if (activeUAS)
            disconnect(activeUAS, 0, this, 0);

        activeUAS = uas;

        connect(activeUAS, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBattery(UASInterface*,double,double,int)));
        connect(activeUAS, SIGNAL(dropRateChanged(int,float)), this, SLOT(updateReceiveLoss(int,float)));
        connect(activeUAS, SIGNAL(loadChanged(UASInterface*, double)), this, SLOT(updateCPULoad(UASInterface*,double)));
        connect(activeUAS, SIGNAL(errCountChanged(int,QString,QString,int)), this, SLOT(updateErrorCount(int,QString,QString,int)));
        connect(activeUAS, SIGNAL(remoteControlRSSIChanged(float)), this, SLOT(updateRSSI(float)));
        connect(activeUAS, SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)), this, SLOT(updateGpsAcc(int,QString,QString,QVariant,quint64)));
        connect(activeUAS, SIGNAL(gpsLocalizationChanged(UASInterface*,int)), this, SLOT(updateGpsFix(UASInterface*,int)));
    }
}

void UASInfoWidget::updateBattery(UASInterface* uas, double voltage, double percent, int seconds)
{
    setVoltage(uas, voltage);
    setChargeLevel(uas, percent);
    setTimeRemaining(uas, seconds);
}

void UASInfoWidget::updateErrorCount(int uasid, QString component, QString device, int count)
{
    //qDebug() << __FILE__ << __LINE__ << activeUAS->getUASID() << "=" << uasid;
    if (activeUAS->getUASID() == uasid) {
        errors.remove(component + ":" + device);
        errors.insert(component + ":" + device, count);
    }
}

/**
 *
 */
void UASInfoWidget::updateCPULoad(UASInterface* uas, double load)
{
    if (activeUAS == uas) {
        this->load = load;
    }
}

void UASInfoWidget::updateReceiveLoss(int uasId, float receiveLoss)
{
    Q_UNUSED(uasId);
    this->receiveLoss = this->receiveLoss * 0.8f + receiveLoss * 0.2f;
}

/**
  The send loss is typically calculated on the GCS based on packets
  that were received scrambled from the MAV
 */
void UASInfoWidget::updateSendLoss(int uasId, float sendLoss)
{
    Q_UNUSED(uasId);
    this->sendLoss = this->sendLoss * 0.8f + sendLoss * 0.2f;
}

void UASInfoWidget::updateRSSI(float rssi)
{
    if (rssi >= 99.0f)
        rssi = 100;
    this->rssi = rssi;
}

void UASInfoWidget::updateGpsFix(UASInterface* uas, const int fix) {
    if (activeUAS == uas)
        gpsFixType = fix;
}

void UASInfoWidget::updateGpsAcc(const int uasId, const QString &name, const QString &unit, const QVariant val, const quint64 msec)
{
    Q_UNUSED(unit);
    Q_UNUSED(msec);
    if (activeUAS->getUASID() != uasId)
        return;
    if (name.contains(QString("eph")))
        gpsEph = val.toFloat()/100.0f;
    else if (name.contains(QString("epv")))
        gpsEpv = val.toFloat()/100.0f;

}

void UASInfoWidget::setVoltage(UASInterface* uas, double voltage)
{
    if (activeUAS == uas)
        this->voltage = voltage;
}

void UASInfoWidget::setChargeLevel(UASInterface* uas, double chargeLevel)
{
    if (activeUAS == uas) {
        this->chargeLevel = chargeLevel;
    }
}

void UASInfoWidget::setTimeRemaining(UASInterface* uas, double seconds)
{
    if (activeUAS == uas) {
        this->timeRemaining = seconds;
    }
}

void UASInfoWidget::refresh()
{
    QString text;
    QString color;

    ui.voltageLabel->setText(QString::number(this->voltage, 'f', voltageDecimals));
    ui.batteryBar->setValue(qMax(0,qMin(static_cast<int>(this->chargeLevel), 100)));

    ui.loadLabel->setText(QString::number(this->load, 'f', loadDecimals));
    ui.loadBar->setValue(qMax(0, qMin(static_cast<int>(this->load), 100)));

    ui.receiveLossBar->setValue(qMax(0, qMin(static_cast<int>(receiveLoss), 100)));
    ui.receiveLossLabel->setText(QString::number(receiveLoss, 'f', 2));

    ui.sendLossBar->setValue(sendLoss);
    ui.sendLossLabel->setText(QString::number(sendLoss, 'f', 2));

    ui.rssiLabel->setText(text.sprintf("%6.2f", this->rssi));
    ui.rssiBar->setValue(qMax(0, qMin(static_cast<int>(this->rssi), 99)));

    text = "No fix";
    color = "red";
    if (gpsFixType == 2) {
        text = "2D fix";
        color = "yellow";
    }
    else if (gpsFixType == 3) {
        text = "3D fix";
        color = "green";
    }
    ui.fixTypeLabel->setText(text);
    ui.fixTypeLabel->setStyleSheet("font-weight: bold; color: " + color + ";");

    if (gpsEph > 0.0f)
        ui.haccLabel->setText("Hacc: " + QString::number(gpsEph, 'f', 2) + "m");
    if (gpsEpv > 0.0f)
        ui.vaccLabel->setText("Vacc: " + QString::number(gpsEpv, 'f', 2) + "m");

    QString errorString;
    QMapIterator<QString, int> i(errors);
    while (i.hasNext()) {
        i.next();
        errorString += QString(i.key() + ": %1 ").arg(i.value());

        // FIXME
        errorString.replace("IMU:", "");
    }
    ui.errorLabel->setText(errorString);
}


