#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QLayout>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QDockWidget>
#include <QSettings>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDesktopServices>
#include <QVariant>
#include <QToolBar>

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

#include "drone.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qgc.h"
#include "uasmanager.h"
#include "linkmanager.h"
#include "seriallink.h"
#include "mavlinkprotocol.h"
#include "uavconfig.h"
#include "parameterinterface.h"
#include "aqpramwidget.h"
#include "compasswidget.h"
#include "hudwidget.h"

static MainWindow* _instance = NULL;   //MainWindow singleton

MainWindow* MainWindow::instance(void)
{
    return _instance;
}

MainWindow::MainWindow(QWidget *parent) :
    changed(true),
    batteryPercent(0),
    batteryVoltage(0),
    systemArmed(false),
    paramaq(NULL),
    autoReconnect(false),
    styleFileName(""),
    connectFlag(true),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //UAV Config Widget
    config = new UAVConfig();
    ui->mainConfig->setWidget(config);

    //Init action connection + build common widgets
    initActionsConnections();
    connectCommonActionsWidgets();

    // Populate link menu
    QList<LinkInterface*> links = LinkManager::instance()->getLinks();
    foreach(LinkInterface* link, links)
    {
        this->addLink(link);
    }
    connect(LinkManager::instance(), SIGNAL(newLink(LinkInterface*)), this, SLOT(addLink(LinkInterface*)));

    // Load style sheet
    loadStyle();

    // Set the toolbar to be updated every 2s
    connect(&updateViewTimer, SIGNAL(timeout()), this, SLOT(updateToolBarView()));
    updateViewTimer.start(2000);

    //Connect Button & TabWidgets
    connect(ui->btn_RADIO, SIGNAL(clicked()), config, SLOT(TabRadio()));
    connect(ui->btn_IMU, SIGNAL(clicked()), config, SLOT(TabIMU()));
    connect(ui->btn_MOTOR, SIGNAL(clicked()), config, SLOT(TabMotor()));
    connect(ui->btn_OSD, SIGNAL(clicked()), config, SLOT(TabOSD()));
    connect(ui->btn_PID, SIGNAL(clicked()), config, SLOT(TabPID()));
    connect(ui->btn_UPGRADE, SIGNAL(clicked()), config, SLOT(TabUpgrade()));
    connect(ui->btn_CHART, SIGNAL(clicked()), config, SLOT(TabChart()));
    connect(ui->btn_BLHeli, SIGNAL(clicked()), config, SLOT(TabBLHeli()));
    connect(config, SIGNAL(TabClicked(int)), this, SLOT(updateUIButton(int)));

    //First init with Tab_Index(0)
    updateUIButton(0);
}

MainWindow::~MainWindow()
{
    foreach(LinkInterface* link,LinkManager::instance()->getLinks())
    {
        int linkIndex = LinkManager::instance()->getLinks().indexOf(link);
        int linkID = LinkManager::instance()->getLinks().at(linkIndex)->getId();

        foreach (QAction* act, listLinkMenuActions())
        {
            if (act->data().toInt() == linkID)
                ui->menuWidgets->removeAction(act);
        }
        link->disconnect();
        LinkManager::instance()->removeLink(link);
    }
    delete ui;
}


void MainWindow::initActionsConnections()
{
    //Move protocol outside UI
    mavlink     = new MAVLinkProtocol();
    connect(mavlink, SIGNAL(protocolStatusMessage(QString,QString)), this, SLOT(showCriticalMessage(QString,QString)), Qt::QueuedConnection);

    //Add action to ToolBar
    ui->actionConnect->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    //Add Widget: OnBoard Parameter: test
    parametersDockWidget = new QDockWidget(tr("Onboard Parameters"), this);
    parametersDockWidget->setWidget( new ParameterInterface(this) );
    parametersDockWidget->setObjectName("PARAMETER_INTERFACE_DOCKWIDGET");
    addTool(parametersDockWidget, tr("Onboard Parameters"), Qt::RightDockWidgetArea);
    parametersDockWidget->hide();

//    //Primary Flight Display on Pitch + Roll
//    ui->scrollArea_heading->setWidget(new HUDWidget(this));

//    //Compass Display on Yaw
//    ui->scrollArea_Compass->setWidget(new CompassWidget(this));

    /**
     * ===== Toolbar Status =====
     */
    toolBarTimeoutLabel = new QLabel(tr("NOT CONNECTED"), this);
    toolBarTimeoutLabel->setToolTip(tr("System connection status."));
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
    ui->mainToolBar->addWidget(toolBarTimeoutLabel);

    toolBarSafetyLabel = new QLabel(tr("SAFE"), this);
    toolBarSafetyLabel->setToolTip(tr("Vehicle safety state"));
    toolBarSafetyLabel->setStyleSheet(QString("QLabel { padding: 2px; color: white; }"));
    ui->mainToolBar->addWidget(toolBarSafetyLabel);

    toolBarBatteryBar = new QProgressBar(this);
    toolBarBatteryBar->setMinimum(0);
    toolBarBatteryBar->setMaximum(100);
    toolBarBatteryBar->setMinimumWidth(20);
    toolBarBatteryBar->setMaximumWidth(100);
    toolBarBatteryBar->setToolTip(tr("Battery charge level"));
    toolBarBatteryBar->setStyleSheet(QString("QLabel { padding: 2px; color:white; }"));
    ui->mainToolBar->addWidget(toolBarBatteryBar);

    toolBarBatteryVoltageLabel = new QLabel("0.0 V");
    toolBarBatteryVoltageLabel->setToolTip(tr("Battery Voltage"));
    toolBarBatteryVoltageLabel->setObjectName("toolBarBatteryVoltageLabel");
    toolBarBatteryVoltageLabel->setStyleSheet(QString("QLabel { padding: 2px; color: white; }"));
    ui->mainToolBar->addWidget(toolBarBatteryVoltageLabel);

    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));
}

void MainWindow::addTool(QDockWidget* widget, const QString& title, Qt::DockWidgetArea area)
{
    QAction* tempAction = ui->menuWidgets->addAction(title);

    tempAction->setCheckable(true);
    QVariant var;
    var.setValue((QWidget*)widget);
    tempAction->setData(var);
    connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
    connect(widget, SIGNAL(visibilityChanged(bool)), tempAction, SLOT(setChecked(bool)));
    tempAction->setChecked(widget->isVisible());
    addDockWidget(area, widget);
    widget->setMinimumWidth(250);

}

void MainWindow::showTool(bool show)
{
    QAction* act = qobject_cast<QAction *>(sender());
    QWidget* widget = act->data().value<QWidget *>();
    widget->setVisible(show);
}

//Connect common actions
void MainWindow::connectCommonActionsWidgets()
{    
//    connect(ui->actionReadParam, SIGNAL(triggered()), config, SLOT(loadParametersToUI()));
    connect(ui->actionSave, SIGNAL(triggered()),config, SLOT(saveAQSetting()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(addLinkImmediately()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(addLink()));

    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(UASCreated(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(UASDeleted(UASInterface*)), this, SLOT(UASDeleted(UASInterface*)));

    if (infoDockWidget && infoDockWidget->widget())
    {
        connect(mavlink, SIGNAL(receiveLossChanged(int,float)),infoDockWidget->widget(), SLOT(updateSendLoss(int, float)));
    }
}

QList<QAction *> MainWindow::listLinkMenuActions()
{
    return ui->menuWidgets->actions();
}

QAction *MainWindow::getActionByLink(LinkInterface *link)
{
    QAction *ret = NULL;
    int linkIndex = LinkManager::instance()->getLinks().indexOf(link);
    int linkID = LinkManager::instance()->getLinks().at(linkIndex)->getId();

    foreach (QAction* act, listLinkMenuActions())
    {
        if (act->data().toInt() == linkID)
            return act;
    }
    return ret;
}

/*
 * ============ Add a communication link ===============
 */
void MainWindow::addLinkImmediately()
{
    if (connectFlag)
    {
        SerialLink* link = new SerialLink();
        LinkManager::instance()->add(link);
        LinkManager::instance()->addProtocol(link, mavlink);

        if (link->isPortHandleValid())
        {
            if (link->connect())
            {
                ui->actionConfigure->setEnabled(false);
                connect(&updateAddLinkImm, SIGNAL(timeout()), this, SLOT(updateBattery()));
                updateAddLinkImm.start(500);
                connectFlag = false;
            }
        }
        else
        {
            LinkManager::instance()->removeLink(link);
            MainWindow::instance()->showCriticalMessage(tr("Error!"), tr("Please plugin your device to begin."));
        }
    }
    else
    {
        closeSerialPort();
        connectFlag = true;
    }
}

void MainWindow::addLink()
{
    SerialLink* link = new SerialLink();
    LinkManager::instance()->add(link);
    LinkManager::instance()->addProtocol(link, mavlink);

    QAction* act = getActionByLink(link);
    if (act)
        act->trigger();

    if (link->isPortHandleValid())
    {
        ui->actionConfigure->setEnabled(false);
        connect(&updateAddLink, SIGNAL(timeout()), this, SLOT(updateBattery()));
        updateAddLink.start(500);
    }
}

void MainWindow::addLink(LinkInterface *link)
{
    LinkManager::instance()->add(link);
    LinkManager::instance()->addProtocol(link, mavlink);

    if (!getActionByLink(link))
    {
        CommConfigurationWindow* commWidget = new CommConfigurationWindow(link, mavlink, this);
        QAction* action = commWidget->getAction();
        ui->menuWidgets->addAction(action);

        // Error handling
        connect(link, SIGNAL(communicationError(QString,QString)), this, SLOT(showCriticalMessage(QString,QString)), Qt::QueuedConnection);
    }
}

void MainWindow::setActiveUAS(UASInterface *uas)
{
    // Do nothing if system is the same or NULL
    if (uas == NULL) return;

    connect(uas, SIGNAL(statusChanged(UASInterface*,QString,QString)), this, SLOT(updateState(UASInterface*, QString,QString)));
    connect(uas, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));
    connect(uas, SIGNAL(armingChanged(bool)), this, SLOT(updateArmingState(bool)));
    connect(uas,SIGNAL(heartbeatTimeout(bool,uint)),this,SLOT(heartbeatTimeout(bool,uint)));
    systemArmed = uas->isArmed();
}

void MainWindow::UASSpecsChanged(int uas)
{
    UASInterface* activeUAS = UASManager::instance()->getActiveUAS();
    if (activeUAS)
    {
        if (activeUAS->getUASID() == uas)
        {
            ui->menuWidgets->setTitle(activeUAS->getUASName());
        }
    }
    else
    {
        // Last system deleted
        ui->menuWidgets->setTitle(tr("No System"));
        ui->menuWidgets->setEnabled(false);
    }
}

void MainWindow::UASCreated(UASInterface* uas)
{
    if (!uas)
        return;

    connect(uas, SIGNAL(systemSpecsChanged(int)), this, SLOT(UASSpecsChanged(int)));
}

void MainWindow::UASDeleted(UASInterface *uas)
{
    QAction* act;
    QList<QAction*> actions = ui->menuWidgets->actions();
    foreach (act, actions) {
        if (act->text().contains(uas->getUASName()))
            ui->menuWidgets->removeAction(act);
    }
}

void MainWindow::closeSerialPort()
{
    foreach(LinkInterface* link,LinkManager::instance()->getLinks())
    {
        int linkIndex = LinkManager::instance()->getLinks().indexOf(link);
        int linkID = LinkManager::instance()->getLinks().at(linkIndex)->getId();

        foreach (QAction* act, listLinkMenuActions())
        {
            if (act->data().toInt() == linkID)
                ui->menuWidgets->removeAction(act);
        }

        link->disconnect();
        //if (link->isRunning()) link->terminate();
        //link->wait();
        //LinkManager::instance()->removeLink(link);
        //link->deleteLater();
    }
    ui->actionConfigure->setEnabled(true);
}

/*
 * ================================================
 * ========== Update CONNECTION status ============
 * ================================================
 */
void MainWindow::heartbeatTimeout(bool timeout, unsigned int ms)
{
    if (timeout)
    {
        if (ms > 10000)
        {
            toolBarTimeoutLabel->setText(tr("DISCONNECTED"));
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
            return;
        }
        else
        {
            if ((ms / 1000) % 2 == 0)
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
            }
            else
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
            }
            toolBarTimeoutLabel->setText(tr("CONNECTION LOST: %1 s").arg((ms / 1000.0f), 2, 'f', 1, ' '));
        }
    }
    else
    {

        toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
        toolBarTimeoutLabel->setText(tr("CONNECTED"));
    }
}

void MainWindow::updateBatteryRemaining(UASInterface *uas, double voltage, double percent, int seconds)
{
    Q_UNUSED(uas);
    Q_UNUSED(seconds);
    if (batteryPercent != percent || batteryVoltage != voltage) changed = true;
    batteryPercent = percent;
    batteryVoltage = voltage;
}

void MainWindow::updateArmingState(bool armed)
{
    systemArmed = armed;
    changed = true;
    updateToolBarView();
}

void MainWindow::updateBattery()
{
    toolBarBatteryVoltageLabel->setText(tr("%1 V").arg(batteryVoltage, 4, 'f', 1, ' '));
}

/*
 * ================================================
 * ============== Load Style Sheet ===============
 * ================================================
 */
void MainWindow::loadStyle() //not use yet
{
    QString path = "/styles/";
    QString stylePath = QApplication::applicationDirPath();
    stylePath.append(path);
    QString styleFileName_tmp = stylePath + "style-Sep.css";

    QFile *styleSheet;
    if (!styleFileName_tmp.isEmpty())
        styleSheet = new QFile(styleFileName_tmp);
    else
        showCriticalMessage(tr("No style sheet"), tr("Please make sure you have style file in your directory"));

    if (styleSheet->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString style = QString(styleSheet->readAll());
        qApp->setStyleSheet(style);
    }
    else
        showCriticalMessage(tr("VSKConfigUAV can't load a new style"), tr("Stylesheet file %1 was not readable").arg(stylePath));

    delete styleSheet;
}

void MainWindow::updateUIButton(int i)
{
    switch (i) {
    case 0:
    {
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 1:
    {
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 2:
    {
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 3:
    {
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 4:
    {
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 5:
    {
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 6:
    {
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000; border-right: 1px solid; border-bottom: 1px solid;}"));
        break;
    }
    case 7:
    {
        ui->btn_BLHeli->setStyleSheet(QString("QToolButton {background-color: #E7E7E7; color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_MOTOR->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_IMU->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_PID->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_CHART->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_OSD->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_RADIO->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid; border-bottom: 1px solid;}"));
        ui->btn_UPGRADE->setStyleSheet(QString("QToolButton {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(89, 89, 89, 255), stop:0.693182 rgba(129, 129, 129, 255), stop:0.98 rgba(185, 185, 185, 255), stop:1 rgba(0, 0, 0, 0)); color: #FFFFFF; border: 0px;} QToolButton:hover{ background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0113636 rgba(172, 172, 172, 255), stop:0.539773 rgba(255, 255, 255, 255), stop:0.977273 rgba(172, 172, 172, 255));color: #000000;border-top: 1px solid; border-right: 1px solid;}"));
    }
    default:
    {
        break;
    }
        break;
    }

}

void MainWindow::showMessage(const QString &title, const QString &message, const QString &details, const QString severity)
{
    QMessageBox msgBox(this);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.setParent(0);
    if (severity == "critical")
        msgBox.setIcon(QMessageBox::Critical);
    else if  (severity == "warning")
        msgBox.setIcon(QMessageBox::Warning);
    else
        msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(title.leftJustified(300, ' '));
    msgBox.setInformativeText(message);
    if (!details.isEmpty())
        msgBox.setDetailedText(details);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::showCriticalMessage(const QString& title, const QString& message)
{
    showMessage(title, message, "", "critical");
}

void MainWindow::updateState(UASInterface *system, QString name, QString description)
{
    connect(system, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));

    Q_UNUSED(description);

    if (state != name)
        changed = true;
    state = name;
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin-left: 300px; padding: 2px; color: white; }"));
    toolBarTimeoutLabel->setText(tr("CONNECTION"));

    // immediately update toolbar
    updateToolBarView();
}

/*
 * ============ Update TOOLBAR View ===============
 */

void MainWindow::updateToolBarView()
{
    if (!changed) return;

    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    toolBarBatteryBar->setValue(batteryPercent);

    if (systemArmed)
    {
        toolBarSafetyLabel->setStyleSheet(QString("QLabel {padding: 2px; color: white; }").arg(QGC::colorRed.name()));
        toolBarSafetyLabel->setText(tr("ARMED"));
    }
    else
    {
        toolBarSafetyLabel->setStyleSheet(QString("QLabel {padding: 2px; color: white; }"));
        toolBarSafetyLabel->setText(tr("SAFE"));
    }
    changed = false;
}
