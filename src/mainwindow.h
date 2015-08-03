#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QPointer>
#include <QMainWindow>
#include <QToolBar>
#include <QProgressBar>
#include <QSettings>
#include <QLabel>
#include <qlist.h>
#include <cstring>

#include "common/common.h"
#include "common/mavlink.h"
#include "common/mavlink_types.h"
#include "ui_mainwindow.h"
#include "linkinterface.h"
#include "uasinterface.h"
#include "primaryflightdisplay.h"
#include "parameterinterface.h"
#include "mavlinkdecoder.h"
#include "commconfigurationwindow.h"
#include "drone.h"
#include "compasswidget.h"

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QtQml>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QOpenGLContext>
#include <QGraphicsObject>
#include <QTimer>

namespace Ui {
class MainWindow;
}
class Drone;
class Window;
class GLWidget;
class ToolBar;
class MAVLinkProtocol;
class MAVLinkDecoder;
class UAVConfig;

class UASInterface;
class SerialLink;
class MAVLinkMessageSender;
class ParameterInterface;
class UASInfoWidget;
class CommConfigurationWindow;
class PrimaryFlightDisplay;
class HDDisplay;
class CompassWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// @brief Returns the MainWindow singleton. Will not create the MainWindow if it has not already
    ///         been created.
    static MainWindow* instance(void);

    MAVLinkProtocol* getMAVLink()
    {
        return mavlink;
    }
    QList<QAction*> listLinkMenuActions(void);

    QAction *getActionByLink(LinkInterface *link);


protected:
    bool connectFlag;
    QMutex dataMutex;
    int lastIndex[256][256];	///< Store the last received sequence ID for each system/componenet pair
    int totalReceiveCounter;
    int totalLossCounter;
    int currReceiveCounter;
    int currLossCounter;
    QMutex receiveMutex;
    quint64 bitsReceivedTotal;
    uint8_t sys_mode;

    QString styleFileName;


    // Status ToolBar define
    QLabel* toolBarTimeoutLabel;
    QLabel* toolBarSafetyLabel;
    QLabel* toolBarStateLabel;
    QLabel* toolBarMessageLabel;


    QProgressBar* toolBarBatteryBar;
    QLabel* toolBarBatteryVoltageLabel;
    float batteryPercent;
    float batteryVoltage;
    bool changed;
    bool systemArmed;
    QTimer updateViewTimer;

    QPointer<QDockWidget> mavlinkSenderWidget;
    QPointer<QDockWidget> HeadingWidget;
    QPointer<QDockWidget> headDown1DockWidget;

    QPointer<QDockWidget> infoDockWidget;
    QPointer<QDockWidget> parametersDockWidget;
    //QPointer<QDockWidget> headingWidget;


    QPointer<MAVLinkDecoder> mavlinkDecoder;
    QPointer<ToolBar> toolBar;

    void addTool(QDockWidget* widget, const QString& title, Qt::DockWidgetArea location=Qt::RightDockWidgetArea);
    void connectCommonWidgets();
    void connectCommonActions();
    void initActionsConnections();

    bool autoReconnect;
        QString state;


    MAVLinkProtocol* mavlink;
    AQParamWidget* paramaq;
    UAVConfig *config; //main tab configuration VSK
    QSettings setting;
    QQuickView view;
    Drone drone;
signals:
    /**
     * @brief This signal is emitted instantly when the link is connected
     **/
    void connected();
    void portError();
    void batteryChanged(double voltage, double percent);
    void requestParameterRefreshed();


public slots:
//    virtual void readData();
    void showTool(bool visible);

    /** @brief Add a communication link */
    void addLink();
    void addLink(LinkInterface* link);
    void addLinkImmediately();

    /** @brief Shows an info or warning message */
    void showMessage(const QString &title, const QString &message, const QString &details, const QString severity = "info");
    /** @brief Shows a critical message as popup or as widget */
    void showCriticalMessage(const QString& title, const QString& message);

    /** @brief Set the system state */
    void updateState(UASInterface* system, QString name, QString description);

    /** @brief Add a new UAS */
    void UASCreated(UASInterface* uas);
    /** @brief Set the system that is currently displayed by this widget */
    void setActiveUAS(UASInterface* active);
    /** @brief Update system specs of a UAS */
    void UASSpecsChanged(int uas);
    /** Delete an UAS */
    void UASDeleted(UASInterface* uas);


    /** @brief Repaint widgets */
    void updateView();
    void updateBattery();
    void closeSerialPort();


    /** @brief Update connection timeout time */
    void heartbeatTimeout(bool timeout, unsigned int ms);
    /** @brief Update battery charge state */
    void updateBatteryRemaining(UASInterface* uas, double voltage, double percent, int seconds);
    /** @brief Update arming state */
    void updateArmingState(bool armed);

    void loadStyle();

public:
    Ui::MainWindow *ui;
};

//Q_DECLARE_METATYPE( initActionsConnections )

#endif // MAINWINDOW_H
