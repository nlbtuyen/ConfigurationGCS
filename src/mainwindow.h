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
#include "mavlink_types.h"
#include "ui_mainwindow.h"
#include "linkinterface.h"
#include "uasinterface.h"
#include "parameterinterface.h"
#include "mavlinkdecoder.h"
#include "commconfigurationwindow.h"
#include "compasswidget.h"
#include "hudwidget.h"
#include "uavconfig.h"


namespace Ui {
class MainWindow;
}

class ToolBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //   Returns the MainWindow singleton. Will not create the MainWindow if it has not already been created.
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
    int lastIndex[256][256];	// Store the last received sequence ID for each system/componenet pair
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
    QTimer updateAddLink;
    QTimer updateAddLinkImm;

    QPointer<QDockWidget> mavlinkSenderWidget;
    QPointer<QDockWidget> HeadingWidget;
    QPointer<QDockWidget> headDown1DockWidget;
    QPointer<QDockWidget> infoDockWidget;
    QPointer<QDockWidget> parametersDockWidget;
    QPointer<QDockWidget> testWidget;
    QPointer<MAVLinkDecoder> mavlinkDecoder;
    QPointer<ToolBar> toolBar;

    void addTool(QDockWidget* widget, const QString& title, Qt::DockWidgetArea location=Qt::RightDockWidgetArea);
    void connectCommonActionsWidgets();
    void initActionsConnections();

    bool autoReconnect;
    QString state;

    MAVLinkProtocol* mavlink;
    AQParamWidget* paramaq;
    UAVConfig *config; //main tab configuration VSK
    QSettings setting;

signals:
    void connected();
    void portError();
    void batteryChanged(double voltage, double percent);
    void requestParameterRefreshed();


public slots:
    void showTool(bool visible);

    // Add a communication link
    void addLink();
    void addLink(LinkInterface* link);
    void addLinkImmediately();
    void closeSerialPort();

    // Shows an info or warning message
    void showMessage(const QString &title, const QString &message, const QString &details, const QString severity = "info");
    void showCriticalMessage(const QString& title, const QString& message);

    // Set the system state
    void updateState(UASInterface* system, QString name, QString description);
    void updateBattery();

    // Control UAS
    void UASCreated(UASInterface* uas);
    void setActiveUAS(UASInterface* active);
    void UASSpecsChanged(int uas);
    void UASDeleted(UASInterface* uas);

    // Status
    void heartbeatTimeout(bool timeout, unsigned int ms); // Update connection timeout time
    void updateBatteryRemaining(UASInterface* uas, double voltage, double percent, int seconds); // Update battery charge state
    void updateArmingState(bool armed);

    // Style
    void loadStyle();
    void updateToolBarView();
    void updateUIButton(int i);

    // Test Board Dialog
    void addTestBoardDialog();

public:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
