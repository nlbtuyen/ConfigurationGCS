#include "uasinfo.h"
#include "ui_uasinfo.h"
#include <QTimer>
#include <QDir>
#include <cstdlib>
#include <cmath>

#include <QDebug>
QT_USE_NAMESPACE

uasInfo::uasInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::uasInfo)
{
    ui->setupUi(this);
//this->setVisible(false);
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
//    errors = QMap<QString, int>();

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    updateTimer->start(updateInterval);
}

uasInfo::~uasInfo()
{
    delete ui;
}

void uasInfo::refresh()
{
    QString text;
    QString color;

    ui->voltageLabel->setText(QString::number(this->voltage, 'f', voltageDecimals));
    ui->batteryBar->setValue(qMax(0,qMin(static_cast<int>(this->chargeLevel), 100)));

    ui->loadLabel->setText(QString::number(this->load, 'f', loadDecimals));
    ui->loadBar->setValue(qMax(0, qMin(static_cast<int>(this->load), 100)));

    ui->receiveLossBar->setValue(qMax(0, qMin(static_cast<int>(receiveLoss), 100)));
    ui->receiveLossLabel->setText(QString::number(receiveLoss, 'f', 2));

    ui->sendLossBar->setValue(sendLoss);
    ui->sendLossLabel->setText(QString::number(sendLoss, 'f', 2));

    ui->rssiLabel->setText(text.sprintf("%6.2f", this->rssi));
    ui->rssiBar->setValue(qMax(0, qMin(static_cast<int>(this->rssi), 99)));

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
    ui->fixTypeLabel->setText(text);
    ui->fixTypeLabel->setStyleSheet("font-weight: bold; color: " + color + ";");

    if (gpsEph > 0.0f)
        ui->haccLabel->setText("Hacc: " + QString::number(gpsEph, 'f', 2) + "m");
    if (gpsEpv > 0.0f)
        ui->vaccLabel->setText("Vacc: " + QString::number(gpsEpv, 'f', 2) + "m");

//    QString errorString;
//    QMapIterator<QString, int> i(errors);
//    while (i.hasNext()) {
//        i.next();
//        errorString += QString(i.key() + ": %1 ").arg(i.value());

//        // FIXME
//        errorString.replace("IMU:", "");


//    }
//    ui->errorLabel->setText(errorString);
}
