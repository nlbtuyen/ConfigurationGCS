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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qgc.h"

#include "uasmanager.h"
#include "linkmanager.h"
#include "seriallink.h"

#include "mavlinkprotocol.h"
#include "mavlinkdecoder.h"

#include "uavconfig.h"
#include "qtoolbar.h"
#include "parameterinterface.h"
#include "mavlinkmessagesender.h"

#include "uasinfowidget.h"
#include "aqpramwidget.h"
#include "primaryflightdisplay.h"
#include "hddisplay.h"

#include "connectiontab.h"


static MainWindow* _instance = NULL;   ///< @brief MainWindow singleton

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
    QMainWindow(parent),
    sys_mode(MAV_MODE_PREFLIGHT),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Set dock options
    setDockOptions(AnimatedDocks | AllowTabbedDocks | AllowNestedDocks);
    statusBar()->setSizeGripEnabled(true);


    config = new UAVConfig();
    ui->mainConfig->setWidget(config);

    //Init Window size
    if(setting.contains(getWindowGeometryKey()))
    {
        //REstore the window geometry
        restoreGeometry(setting.value(getWindowGeometryKey()).toByteArray());
        show();
    }
    else
    {
        //Ajust the size
        const int screenWidth = 800;
        const int screenHeight = 600;

        if (screenWidth <800 || screenHeight <600) {
            showMaximized();
        }
        else{
            resize(qMin(screenWidth,800), qMin(screenHeight,600));
            show();
        }
    }

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

    loadStyle();
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
        if (link->isRunning()) link->terminate();
        link->wait();
        LinkManager::instance()->removeLink(link);
        link->deleteLater();
    }
    delete ui;

}

void MainWindow::initActionsConnections()
{
    //Move protocol outside UI
    mavlink     = new MAVLinkProtocol();
    connect(mavlink, SIGNAL(protocolStatusMessage(QString,QString)), this, SLOT(showCriticalMessage(QString,QString)), Qt::QueuedConnection);
    //    mavlinkDecoder = new MAVLinkDecoder(mavlink, this);
    //    connect(mavlinkDecoder, SIGNAL(textMessageReceived(int, int, int, const QString)), this->, SLOT(receiveTextMessage(int, int, int, const QString)));

    //Add action to ToolBar
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    //Add Widget: Status Detail + OnBoard Parameter
    infoDockWidget = new QDockWidget(tr("Status Details"), this);
    infoDockWidget->setWidget( new UASInfoWidget(this));
    infoDockWidget->setObjectName("UAS_STATUS_DETAILS_DOCKWIDGET");
    addTool(infoDockWidget, tr("Status Details"), Qt::LeftDockWidgetArea);
    infoDockWidget->hide();

    parametersDockWidget = new QDockWidget(tr("Onboard Parameters"), this);
    parametersDockWidget->setWidget( new ParameterInterface(this) );
    parametersDockWidget->setObjectName("PARAMETER_INTERFACE_DOCKWIDGET");
    addTool(parametersDockWidget, tr("Onboard Parameters"), Qt::RightDockWidgetArea);
    parametersDockWidget->hide();

    //===== Toolbar Status =====

    toolBarTimeoutLabel = new QLabel(tr("NOT CONNECTED"), this);
    toolBarTimeoutLabel->setToolTip(tr("System connection status, interval since last message if timed out."));
    toolBarTimeoutLabel->setObjectName("toolBarTimeoutLabel");
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin: 0px 2px; font: 18px; color: %1; }").arg(QColor(Qt::red).name()));
    ui->mainToolBar->addWidget(toolBarTimeoutLabel);

    toolBarBatteryVoltageLabel = new QLabel("0.0 V");
    toolBarBatteryVoltageLabel->setStyleSheet(QString("QLabel {  margin: 0px 2px; font: 18px; color: %1; }").arg(QColor(Qt::green).name()));
    toolBarBatteryVoltageLabel->setToolTip(tr("Battery voltage"));
    toolBarBatteryVoltageLabel->setObjectName("toolBarBatteryVoltageLabel");
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

    //connect(ui->CONNECTIONpushButton,SIGNAL(clicked()),this,SLOT(addLink()));
    //connect(ui->RADIOpushButton,SIGNAL(clicked()),this,SLOT(connectTab()));



    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(addLinkImmediately()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
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
    SerialLink* link = new SerialLink();
    LinkManager::instance()->add(link);
    LinkManager::instance()->addProtocol(link, mavlink);

        if (link->isPortHandleValid())
        {
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);

            connect(&updateViewTimer, SIGNAL(timeout()), this, SLOT(updateBattery()));
            updateViewTimer.start(500);
            link->connect();
        }
        else
        {
            MainWindow::instance()->showCriticalMessage(tr("Error!"), tr("Please plugin your device to begin."));
            closeSerialPort();
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
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        connect(&updateViewTimer, SIGNAL(timeout()), this, SLOT(updateBattery()));
        updateViewTimer.start(500);
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

    //    //update battery
    connect(uas, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));
    //    //update arm or not
    connect(uas, SIGNAL(armingChanged(bool)), this, SLOT(updateArmingState(bool)));

    connect(uas,SIGNAL(heartbeatTimeout(bool,uint)),this,SLOT(heartbeatTimeout(bool,uint)));

    //    //update value
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

    if (infoDockWidget)
    {
        UASInfoWidget *infoWidget = dynamic_cast<UASInfoWidget*>(infoDockWidget->widget());
        if (infoWidget)
        {
            infoWidget->addUAS(uas);
            infoWidget->refresh();
        }
    }
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

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));

}

//Update CONNECTION status
void MainWindow::heartbeatTimeout(bool timeout, unsigned int ms)
{
    if (timeout)
    {
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);

        if (ms > 10000)
        {
            toolBarTimeoutLabel->setText(tr("DISCONNECTED"));
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; color: white }").arg(QGC::colorMagenta.dark(250).name()));
            return;
        }
        else
        {
            if ((ms / 1000) % 2 == 0)
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; color: white }").arg(QGC::colorMagenta.name()));
            }
            else
            {
                toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; color: white }").arg(QGC::colorMagenta.dark(250).name()));
            }
            toolBarTimeoutLabel->setText(tr("CONNECTION LOST: %1 s").arg((ms / 1000.0f), 2, 'f', 1, ' '));
        }
    }
    else
    {

        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);

        toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding:0 3px; background-color: #FF0000; }"));
        toolBarTimeoutLabel->setText(tr("CONNECTION"));
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
    /* important, immediately update */
    updateView();
}

void MainWindow::loadStyle()
{
    QString path = "/styles/";
    QString stylePath = QApplication::applicationDirPath();
    stylePath.append(path);
    QString styleFileName_tmp = stylePath + "style-default.css";

    QFile *styleSheet;
    if (!styleFileName_tmp.isEmpty())
        styleSheet = new QFile(styleFileName_tmp);
    else
        showCriticalMessage(tr("No style sheet"), tr("Please make sure you have style file in your directory"));

    if (styleSheet->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString style = QString(styleSheet->readAll());
        qApp->setStyleSheet(style);
        styleFileName = QFileInfo(*styleSheet).absoluteFilePath();
        //        qDebug() << "Loaded stylesheet:" << styleFileName;
    }
    else
        showCriticalMessage(tr("VSKConfigUAV did not load a new style"), tr("Stylesheet file %1 was not readable").arg(stylePath));

    delete styleSheet;
}

void MainWindow::connectTab()
{
    //    UAVConfig* uav = new UAVConfig();
    //    uav->indexHide(1);
    //    ui->mainTab->setWidget(uav);


}


QString MainWindow::getWindowGeometryKey()
{
    return "_geometry";
}


void MainWindow::showMessage(const QString &title, const QString &message, const QString &details, const QString severity)
{
    QMessageBox msgBox(this);
    //msgBox.setWindowFlags(Qt::Dialog);
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
    //    connect(system, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));

    Q_UNUSED(system);
    Q_UNUSED(description);

    if (state != name)
        changed = true;
    state = name;
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding:0 3px; background-color: #FF0000; color : white; }"));
    toolBarTimeoutLabel->setText(tr("CONNECTION"));

    /* important, immediately update */
    updateView();
}


void MainWindow::updateView()
{
    if (!changed) return;

    //    toolBarBatteryBar->setValue(batteryPercent);
    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

//    if (systemArmed)
//    {
//        toolBarSafetyLabel->setStyleSheet(QString("QLabel {margin: 0px 2px; font: 18px; color: %1; background-color: %2; }").arg(QGC::colorRed.name()).arg(QGC::colorYellow.name()));
//        toolBarSafetyLabel->setText(tr("ARMED"));
//    }
//    else
//    {
//        toolBarSafetyLabel->setStyleSheet("QLabel {margin: 0px 2px; font: 18px; color: #14C814; }");
//        toolBarSafetyLabel->setText(tr("SAFE"));
//    }
//    changed = false;
}

void MainWindow::updateBattery()
{
    toolBarBatteryVoltageLabel->setText(tr("%1 V").arg(batteryVoltage, 4, 'f', 1, ' '));

}
