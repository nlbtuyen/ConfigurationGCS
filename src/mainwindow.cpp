#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
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
    // Set dock options
    statusBar()->setSizeGripEnabled(true);

    //UAV Config Widget
    config = new UAVConfig();
    ui->mainConfig->setWidget(config);

    //Init action connection + build common widgets
    initActionsConnections();
    connectCommonActions();
    connectCommonWidgets();

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
}

MainWindow::~MainWindow()
{
    view.releaseResources();

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

    //Primary Flight Display on Pitch + Roll
    ui->scrollArea_heading->setWidget(new HUDWidget(this));

    //Compass Display on Yaw
    ui->scrollArea_Compass->setWidget(new CompassWidget(this));

    //3D Model: load qml and connect between QML & C++
    view.releaseResources();
    QWidget *container = QWidget::createWindowContainer(&view);
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.clearBeforeRendering();
    view.engine()->clearComponentCache();
    view.rootContext()->setContextProperty("drone",&drone); //connect QML & C++
    view.setSource(QUrl("qrc:/src/main.qml")); //load QML file
    ui->scrollArea_3D->setWidget(container);

    /*
     * ===== Toolbar Status =====
     */

    toolBarTimeoutLabel = new QLabel(tr("NOT CONNECTED"), this);
    toolBarTimeoutLabel->setToolTip(tr("System connection status."));
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #DC5B21; background-color: #D5C79C; font-weight: bold; }"));
    ui->mainToolBar->addWidget(toolBarTimeoutLabel);

    toolBarSafetyLabel = new QLabel(tr("SAFE"), this);
    toolBarSafetyLabel->setToolTip(tr("Vehicle safety state"));
    toolBarSafetyLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #DC5B21; background-color: #D5C79C; font-weight: bold; }"));
    ui->mainToolBar->addWidget(toolBarSafetyLabel);

    toolBarBatteryBar = new QProgressBar(this);
    toolBarBatteryBar->setMinimum(0);
    toolBarBatteryBar->setMaximum(100);
    toolBarBatteryBar->setMinimumWidth(20);
    toolBarBatteryBar->setMaximumWidth(100);
    toolBarBatteryBar->setToolTip(tr("Battery charge level"));
    toolBarBatteryBar->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #DC5B21; background-color: #D5C79C; font-weight: bold;}"));
    ui->mainToolBar->addWidget(toolBarBatteryBar);

    toolBarBatteryVoltageLabel = new QLabel("0.0 V");
    toolBarBatteryVoltageLabel->setToolTip(tr("Battery Voltage"));
    toolBarBatteryVoltageLabel->setObjectName("toolBarBatteryVoltageLabel");
    toolBarBatteryVoltageLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #DC5B21; background-color: #D5C79C; font-weight: bold;}"));
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


void MainWindow::connectCommonWidgets()
{
    if (infoDockWidget && infoDockWidget->widget())
    {
        connect(mavlink, SIGNAL(receiveLossChanged(int,float)),infoDockWidget->widget(), SLOT(updateSendLoss(int, float)));
    }
}

//Connect common actions
void MainWindow::connectCommonActions()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(addLinkImmediately()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(addLink()));

    connect(LinkManager::instance(), SIGNAL(linkRemoved(LinkInterface*)), this, SLOT(closeSerialPort()));
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(UASCreated(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(UASDeleted(UASInterface*)), this, SLOT(UASDeleted(UASInterface*)));

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
                ui->statusBar->showMessage(tr("Connected"));
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
    ///update battery
    connect(uas, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));
    ///update arm or not
    connect(uas, SIGNAL(armingChanged(bool)), this, SLOT(updateArmingState(bool)));
    ///update heartbeat
    connect(uas,SIGNAL(heartbeatTimeout(bool,uint)),this,SLOT(heartbeatTimeout(bool,uint)));
    ///update value
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
        if (link->isRunning()) link->terminate();
        link->wait();
        LinkManager::instance()->removeLink(link);
        link->deleteLater();
    }
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
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
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #70AB8F; background-color: #D5C79C; font-weight: bold; }"));
            return;
        }
        else
        {
            if ((ms / 1000) % 2 == 0)
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel {padding: 2px; font: 16px; color: #DC5B21; background-color: #D5C79C; font-weight: bold; }"));
            }
            else
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #70AB8F; background-color: #D5C79C; font-weight: bold; }"));
            }
            toolBarTimeoutLabel->setText(tr("CONNECTION LOST: %1 s").arg((ms / 1000.0f), 2, 'f', 1, ' '));
        }
    }
    else
    {

        toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #993F17; background-color: #D5C79C; font-weight: bold; }"));
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

/*
 * ================================================
 * ============== Load Style Sheet ===============
 * ================================================
 */
void MainWindow::loadStyle()
{
    QString path = "/styles/";
    QString stylePath = QApplication::applicationDirPath();
    stylePath.append(path);
    QString styleFileName_tmp = stylePath + "style-July.css";

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
        showCriticalMessage(tr("VSKConfigUAV cann't load a new style"), tr("Stylesheet file %1 was not readable").arg(stylePath));

    delete styleSheet;
}

void MainWindow::showMessage(const QString &title, const QString &message, const QString &details, const QString severity)
{
    QMessageBox msgBox(this);
    msgBox.setWindowFlags(Qt::Dialog);
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

    //    Q_UNUSED(system);
    Q_UNUSED(description);

    if (state != name)
        changed = true;
    state = name;
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 2px; font: 16px; color: #993F17; background-color: #D5C79C; font-weight: bold;  }"));
    toolBarTimeoutLabel->setText(tr("CONNECTION"));

    // immediately update toolbar
    updateToolBarView();
}

/*
 * ================================================
 * ============ Update TOOLBAR View ===============
 * ================================================
 */

void MainWindow::updateToolBarView()
{
    if (!changed) return;

    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    toolBarBatteryBar->setValue(batteryPercent);

    if (systemArmed)
    {
        toolBarSafetyLabel->setStyleSheet(QString("QLabel {padding: 2px; font: 16px; color: #70AB8F; background-color: #D5C79C; font-weight: bold;  }").arg(QGC::colorRed.name()));
        toolBarSafetyLabel->setText(tr("ARMED"));
    }
    else
    {
        toolBarSafetyLabel->setStyleSheet("QLabel {padding: 2px; font: 16px; color: #993F17; background-color: #D5C79C; font-weight: bold;  }");
        toolBarSafetyLabel->setText(tr("SAFE"));
    }
    changed = false;
}

void MainWindow::updateBattery()
{
    toolBarBatteryVoltageLabel->setText(tr("%1 V").arg(batteryVoltage, 4, 'f', 1, ' '));
}
