#ifndef UASINFOWIDGET_H
#define UASINFOWIDGET_H


#include <QWidget>
#include <QTimer>
#include <QMap>

#include "uasinterface.h"
#include "ui_uasinfo.h"

/**
 * @brief Info indicator for the currently active UAS
 *
 **/
class UASInfoWidget : public QWidget
{
    Q_OBJECT
public:
    UASInfoWidget(QWidget *parent = 0, QString name = "");
    ~UASInfoWidget();

public slots:
    void addUAS(UASInterface* uas);
    void removeUAS(UASInterface* uas);

    void setActiveUAS(UASInterface* uas);

    void updateBattery(UASInterface* uas, double voltage, double percent, int seconds);
    void updateCPULoad(UASInterface* uas, double load);
    /**
     * @brief Set the loss rate of packets received by the MAV.
     * @param uasId UNUSED
     * @param receiveLoss A percentage value (0-100) of how many message the UAS has failed to receive.
     */
    void updateReceiveLoss(int uasId, float receiveLoss);

    /**
     * @brief Set the loss rate of packets sent from the MAV
     * @param uasId UNUSED
     * @param sendLoss A percentage value (0-100) of how many message QGC has failed to receive.
     */
    void updateSendLoss(int uasId, float sendLoss);

    /** @brief Update the error count */
    void updateErrorCount(int uasid, QString component, QString device, int count);

    void updateRSSI(float rssi);
    void updateGpsFix(UASInterface *uas, const int fix);
    void updateGpsAcc(const int uasId, const QString &name, const QString &unit, const QVariant val, const quint64 msec);

    void setVoltage(UASInterface* uas, double voltage);
    void setChargeLevel(UASInterface* uas, double chargeLevel);
    void setTimeRemaining(UASInterface* uas, double seconds);
//    void setBattery(int uasid, BatteryType type, int cells);

//    void valueChanged(int uasid, QString key, double value,quint64 time);
//    void actuatorChanged(UASInterface* uas, int actId, double value);
    void refresh();

protected:

    UASInterface* activeUAS;

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
    QMap<QString, int> errors;
    static const int updateInterval = 100; ///< Refresh interval in milliseconds

    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

private:
    Ui::uasInfo ui;

};

#endif // UASINFOWIDGET_H
