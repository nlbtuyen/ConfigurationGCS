#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include "common/common.h"
#include "common/mavlink.h"
#include "common/mavlink_types.h"
#include "mainwindow.h"
#include "protocolinterface.h"
#include "linkinterface.h"

QT_USE_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
//        bool localEchoEnabled;
    };

    struct Mavlink_Messages {

        int sysid;
        int compid;

        // Heartbeat
        mavlink_heartbeat_t heartbeat;

        // System Status
        mavlink_sys_status_t sys_status;

        // Battery Status
        mavlink_battery_status_t battery_status;

        // Radio Status
        mavlink_radio_status_t radio_status;

        // Local Position
        mavlink_local_position_ned_t local_position_ned;

        // Global Position
        mavlink_global_position_int_t global_position_int;

        // Local Position Target
        mavlink_position_target_local_ned_t position_target_local_ned;

        // Global Position Target
        mavlink_position_target_global_int_t position_target_global_int;

        // HiRes IMU
        mavlink_highres_imu_t highres_imu;

        // Attitude
        mavlink_attitude_t attitude;



    };


    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    Settings settings() const;


private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);


private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
    LinkInterface* link;
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();



};
#endif // SETTINGSDIALOG_H
