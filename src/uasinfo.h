#ifndef UASINFO_H
#define UASINFO_H
#include <QWidget>
#include "mainwindow.h"

QT_USE_NAMESPACE

namespace Ui {
class uasInfo;
}

class uasInfo : public QWidget
{
    Q_OBJECT

public:
    explicit uasInfo(QWidget *parent = 0);
    ~uasInfo();


public slots:
    void refresh();

protected:
    // Configuration variables
    int voltageDecimals;
    int loadDecimals;

    // State variables

    // Voltage
    double voltage;
    double chargeLevel;
    double timeRemaining;
    double load;
    float receiveLoss;
    float sendLoss;
    float rssi;
    float gpsEph;
    float gpsEpv;
    quint8 gpsFixType;
    bool changed;
    QTimer* updateTimer;
    QString name;
    quint64 startTime;
//    QMap<QString, int> errors;
    static const int updateInterval = 800; ///< Refresh interval in milliseconds

//    void showEvent(QShowEvent* event);
//    void hideEvent(QHideEvent* event);

 private:
    Ui::uasInfo *ui;
};

#endif // UASINFO_H
