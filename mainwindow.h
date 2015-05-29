#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QPointer>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QToolBar>
#include "common/common.h"
#include "common/mavlink.h"
#include "common/mavlink_types.h"
#include <QLabel>
#include <QProgressBar>
#include <QSettings>

#include "uasinterface.h"
#include "parameterinterface.h"
#include "mavlinkmessagesender.h"
#include "linkinterface.h"
#include "seriallink.h"
#include "uasinfowidget.h"
#include "toolbar.h"

namespace Ui {
class MainWindow;
}

class ToolBar;
class SerialLink;
class MAVLinkMessageSender;
class ParameterInterface;
class SettingsDialog;
class UAVConfig;
class MAVLinkProtocol;
class UASInfoWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MAVLinkProtocol* getMAVLink()
    {
        return mavlink;
    }

protected:
    QMutex dataMutex;
    void bytesReceived(QByteArray data);
    int lastIndex[256][256];	///< Store the last received sequence ID for each system/componenet pair
    int totalReceiveCounter;
    int totalLossCounter;
    int currReceiveCounter;
    int currLossCounter;
    void receiveMessage(mavlink_message_t message);
    QMutex receiveMutex;
    quint64 bitsReceivedTotal;

    uint8_t sys_mode;

    double latitude;
    double longitude;
    double altitude;
    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;
    double rollspeed;
    double pitchspeed;
    double yawspeed;

    mavlink_message_t receivedMessages[256]; ///< Available / known messages
    mavlink_message_info_t messageInfo[256]; ///< Message information

    static void print_message(const mavlink_message_t *msg);
    static void print_field(const mavlink_message_t *msg, const mavlink_field_info_t *f);
    static void print_one_field(const mavlink_message_t *msg, const mavlink_field_info_t *f, int idx);

    MAVLinkProtocol* mavlink;

    QLabel* symbolLabel;
    QLabel* toolBarNameLabel;
    QLabel* toolBarTimeoutLabel;
    QLabel* toolBarSafetyLabel;
    QLabel* toolBarStateLabel;
    QLabel* toolBarMessageLabel;
    QProgressBar* toolBarBatteryBar;
    QLabel* toolBarBatteryVoltageLabel;
    QPointer<QDockWidget> mavlinkSenderWidget;
    QPointer<QDockWidget> parametersDockWidget;
    QPointer<QDockWidget> infoDockWidget;
    QPointer<ToolBar> toolBar;

    void addTool(QDockWidget* widget, const QString& title, Qt::DockWidgetArea location=Qt::RightDockWidgetArea);

//    QPointer<MAVLinkDecoder> mavlinkDecoder;

public slots:
    virtual void readData();
    void showTool(bool visible);

    /** @brief Add a communication link */
    void addLink();
    void addLink(LinkInterface* link);
    /** @brief Shows an info or warning message */
    void showMessage(const QString &title, const QString &message, const QString &details, const QString severity = "info");
    /** @brief Shows a critical message as popup or as widget */
    void showCriticalMessage(const QString& title, const QString& message);

private slots:
//    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);
//    void updateBattery(double, double);


private:
    void initActionsConnections();
    Ui::MainWindow *ui;
    SettingsDialog *settings;
    QSerialPort *serial;
    UAVConfig *config;
    SerialLink* link;
    QSettings setting;
    //ToolBar* toolBar;


    QString getWindowGeometryKey();


signals:
    /**
     * @brief This signal is emitted instantly when the link is connected
     **/
    void connected();
    void portError();
    void batteryChanged(double voltage, double percent);


};

#endif // MAINWINDOW_H
