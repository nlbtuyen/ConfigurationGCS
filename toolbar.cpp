#include "toolbar.h"
#include "mg.h"
#include "uasmanager.h"
#include "mainwindow.h"
#include "linkmanager.h"

#include <QFileDialog>
#include <QMessageBox>

ToolBar::ToolBar(QWidget *parent):
    QToolBar(parent),
    toggleLoggingAction(NULL),
    logReplayAction(NULL),
    mav(NULL),
    changed(true),
    batteryPercent(0),
    batteryVoltage(0),
    wpId(0),
    wpDistance(0),
    systemArmed(false),
    currentLink(NULL)
{
    setObjectName("QGC_TOOLBAR");

}

void ToolBar::heartbeatTimeout(bool timeout, unsigned int ms)
{
    if (ms > 10000) {
        if (!currentLink || !currentLink->isConnected()) {
            toolBarTimeoutLabel->setText(tr("DISCONNECTED"));
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; }").arg(QGC::colorMagenta.dark(250).name()));
            return;
        }
    }

    // set timeout label visible
    if (timeout)
    {
        // Alternate colors to increase visibility
        if ((ms / 1000) % 2 == 0)
        {
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; }").arg(QGC::colorMagenta.name()));
        }
        else
        {
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: %2; }").arg(QGC::colorMagenta.dark(250).name()));
        }
        toolBarTimeoutLabel->setText(tr("CONNECTION LOST: %1 s").arg((ms / 1000.0f), 2, 'f', 1, ' '));
    }
    else
    {
        // Check if loss text is present, reset once
        if (toolBarTimeoutLabel->text() != "")
        {
            toolBarTimeoutLabel->setStyleSheet(QString(" padding: 0;"));
            toolBarTimeoutLabel->setText("");
        }
    }
}

void ToolBar::createUI() {

//    toggleLoggingAction = new QAction(QIcon(":"), "Logging", this);
//    toggleLoggingAction->setCheckable(true);
//    logReplayAction = new QAction(QIcon(":"), "Replay", this);
//    logReplayAction->setCheckable(false);

//    addAction(toggleLoggingAction);
//    addAction(logReplayAction);

    // CREATE TOOLBAR ITEMS
    // Add internal actions
    // Add MAV widget
    symbolLabel = new QLabel(this);
    //symbolButton->setStyleSheet("QWidget { background-color: #050508; color: #DDDDDF; background-clip: border; }");
    symbolLabel->setObjectName("mavSymbolLabel");
    addWidget(symbolLabel);

    toolBarNameLabel = new QLabel("------", this);
    toolBarNameLabel->setToolTip(tr("Currently controlled vehicle"));
    toolBarNameLabel->setObjectName("toolBarNameLabel");
    addWidget(toolBarNameLabel);

    toolBarTimeoutLabel = new QLabel(tr("NOT CONNECTED"), this);
    toolBarTimeoutLabel->setToolTip(tr("System connection status, interval since last message if timed out."));
    toolBarTimeoutLabel->setObjectName("toolBarTimeoutLabel");
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { background-color: %2; padding: 0 3px; }").arg(QGC::colorMagenta.dark(250).name()));
    addWidget(toolBarTimeoutLabel);

    toolBarSafetyLabel = new QLabel(tr("SAFE"), this);
    toolBarSafetyLabel->setToolTip(tr("Vehicle safety state"));
    toolBarSafetyLabel->setObjectName("toolBarSafetyLabel");
    addWidget(toolBarSafetyLabel);

    toolBarModeLabel = new QLabel("------", this);
    toolBarModeLabel->setToolTip(tr("Vehicle mode"));
    toolBarModeLabel->setObjectName("toolBarModeLabel");
    addWidget(toolBarModeLabel);

    toolBarStateLabel = new QLabel("------", this);
    toolBarStateLabel->setToolTip(tr("Vehicle state"));
    toolBarStateLabel->setObjectName("toolBarStateLabel");
    addWidget(toolBarStateLabel);

    toolBarBatteryBar = new QProgressBar(this);
//    toolBarBatteryBar->setStyleSheet("QProgressBar:horizontal { margin: 0px 4px 0px 0px; border: 1px solid #4A4A4F; border-radius: 4px; text-align: center; padding: 2px; color: #111111; background-color: #111118; height: 10px; } QProgressBar:horizontal QLabel { font-size: 9px; color: #111111; } QProgressBar::chunk { background-color: green; }");
    toolBarBatteryBar->setMinimum(0);
    toolBarBatteryBar->setMaximum(100);
    toolBarBatteryBar->setMinimumWidth(20);
    toolBarBatteryBar->setMaximumWidth(100);
    toolBarBatteryBar->setToolTip(tr("Battery charge level"));
    toolBarBatteryBar->setObjectName("toolBarBatteryBar");
    addWidget(toolBarBatteryBar);

    toolBarBatteryVoltageLabel = new QLabel("xx.x V");
    toolBarBatteryVoltageLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(QColor(Qt::green).name()));
    toolBarBatteryVoltageLabel->setToolTip(tr("Battery voltage"));
    toolBarBatteryVoltageLabel->setObjectName("toolBarBatteryVoltageLabel");
    addWidget(toolBarBatteryVoltageLabel);

//    toolBarWpLabel = new QLabel("WP--", this);
//    toolBarWpLabel->setStyleSheet("QLabel { margin: 0px 2px; font: 18px; color: #3C7B9E; }");
//	toolBarWpLabel->setToolTip(tr("Current mission"));
//    addWidget(toolBarWpLabel);

//    toolBarDistLabel = new QLabel("--- ---- m", this);
//	toolBarDistLabel->setToolTip(tr("Distance to current mission"));
//    addWidget(toolBarDistLabel);

    toolBarMessageLabel = new QLabel(tr("No system messages."), this);
    toolBarMessageLabel->setToolTip(tr("Most recent system message"));
    toolBarMessageLabel->setObjectName("toolBarMessageLabel");
    addWidget(toolBarMessageLabel);

    // DONE INITIALIZING BUTTONS

    // Configure the toolbar for the current default UAS
    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    if (LinkManager::instance()->getLinks().count() > 1)
        addLink(LinkManager::instance()->getLinks().last());
    connect(LinkManager::instance(), SIGNAL(newLink(LinkInterface*)), this, SLOT(addLink(LinkInterface*)));
    connect(LinkManager::instance(), SIGNAL(linkRemoved(LinkInterface*)), this, SLOT(removeLink(LinkInterface*)));

    // Set the toolbar to be updated every 2s
    connect(&updateViewTimer, SIGNAL(timeout()), this, SLOT(updateView()));
    updateViewTimer.start(2000);
}

void ToolBar::setPerspectiveChangeActions(const QList<QAction*> &actions)
{
    if (actions.count())
    {
        group = new QButtonGroup(this);
        group->setExclusive(true);

        for (int i = 0; i < actions.count(); i++)
        {
            // Add last button
            QToolButton *btn = new QToolButton(this);
            // Add first button
            btn->setIcon(actions.at(i)->icon());
            btn->setText(actions.at(i)->text());
            btn->setToolTip(actions.at(i)->toolTip());
            btn->setObjectName(actions.at(i)->objectName());
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setCheckable(true);
            connect(btn, SIGNAL(clicked(bool)), actions.at(i), SIGNAL(triggered(bool)));
            connect(actions.at(i),SIGNAL(triggered(bool)),btn,SLOT(setChecked(bool)));
            connect(actions.at(i),SIGNAL(toggled(bool)),btn,SLOT(setChecked(bool)));
            //btn->setStyleSheet("QToolButton { min-width: 60px; color: #222222; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #A2A3A4, stop: 1 #B6B7B8);  margin-left: -2px; margin-right: -2px; padding-left: 0px; padding-right: 0px; border-radius: 0px; border-top: 1px solid #484848; border-bottom: 1px solid #484848; } QToolButton:checked { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #555555, stop: 1 #787878); color: #DDDDDD; }");
            addWidget(btn);
            group->addButton(btn);
        }

//        // Add last button
//        advancedButton = new QPushButton(this);
//        // Add first button
//        advancedButton->setIcon(QIcon(":/files/images/apps/utilities-system-monitor.svg"));
//        advancedButton->setText(tr("Pro"));
//        advancedButton->setToolTip(tr("Options for advanced users"));
//        advancedButton->setCheckable(true);
//        advancedButton->setStyleSheet("QPushButton { min-width: 60px; font-weight: bold; text-align: left; color: #222222; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #A2A3A4, stop: 1 #B6B7B8);  margin-left: 0px; margin-right: 13px; padding-left: 4px; padding-right: 8px; border-radius: 0px; border : 0px solid blue; border-bottom-right-radius: 6px; border-top-right-radius: 6px; border-right: 1px solid #484848; border-top: 1px solid #484848; border-bottom: 1px solid #484848; } QPushButton:checked { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #555555, stop: 1 #787878); color: #DDDDDD; }");
//        addWidget(advancedButton);
//        group->addButton(advancedButton);

    } else {
        qDebug() << __FILE__ << __LINE__ << "Not enough perspective change actions provided";
    }

    // Add the "rest"
    createUI();
}

void ToolBar::addPerspectiveChangeAction(QAction* action)
{
    insertAction(toggleLoggingAction, action);
}

void ToolBar::setActiveUAS(UASInterface* active)
{
    // Do nothing if system is the same or NULL
    if ((active == NULL) || mav == active) return;

    if (mav)
    {
        // Disconnect old system
        disconnect(mav, 0, this, 0);

    }

    // Connect new system
    mav = active;
    connect(active, SIGNAL(statusChanged(UASInterface*,QString,QString)), this, SLOT(updateState(UASInterface*, QString,QString)));
    connect(active, SIGNAL(modeChanged(int,QString,QString)), this, SLOT(updateMode(int,QString,QString)));
    connect(active, SIGNAL(nameChanged(QString)), this, SLOT(updateName(QString)));
    connect(active, SIGNAL(systemTypeSet(UASInterface*,uint)), this, SLOT(setSystemType(UASInterface*,uint)));
    connect(active, SIGNAL(textMessageReceived(int,int,int,QString)), this, SLOT(receiveTextMessage(int,int,int,QString)));
    connect(active, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));
    connect(active, SIGNAL(armingChanged(bool)), this, SLOT(updateArmingState(bool)));
    connect(active, SIGNAL(heartbeatTimeout(bool, unsigned int)), this, SLOT(heartbeatTimeout(bool,unsigned int)));


    // Update all values once
    systemName = mav->getUASName();
    systemArmed = mav->isArmed();
    toolBarNameLabel->setText(mav->getUASName());
    toolBarNameLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(mav->getColor().name()));
//    symbolLabel->setStyleSheet(QString("QLabel {background-color: %1;}").arg(mav->getColor().name()));
    toolBarModeLabel->setText(mav->getShortMode());
    toolBarStateLabel->setText(mav->getShortState());
    toolBarTimeoutLabel->setStyleSheet(QString(" padding: 0;"));
    toolBarTimeoutLabel->setText("");
    setSystemType(mav, mav->getSystemType());
}

void ToolBar::createCustomWidgets()
{

}

void ToolBar::updateArmingState(bool armed)
{
    systemArmed = armed;
    changed = true;
    /* important, immediately update */
    updateView();
}

void ToolBar::updateView()
{
    if (!changed) return;
//    toolBarDistLabel->setText(tr("%1 m").arg(wpDistance, 6, 'f', 2, '0'));
//    toolBarWpLabel->setText(tr("WP%1").arg(wpId));
    toolBarBatteryBar->setValue(batteryPercent);
    toolBarBatteryVoltageLabel->setText(tr("%1 V").arg(batteryVoltage, 4, 'f', 1, ' '));
    toolBarStateLabel->setText(state);
    toolBarModeLabel->setText(mode);
    toolBarNameLabel->setText(systemName);
    toolBarMessageLabel->setText(lastSystemMessage);

    if (systemArmed)
    {
        toolBarSafetyLabel->setStyleSheet(QString("QLabel { color: %1; background-color: %2; }").arg(QGC::colorRed.name()).arg(QGC::colorYellow.name()));
        toolBarSafetyLabel->setText(tr("ARMED"));
    }
    else
    {
        toolBarSafetyLabel->setStyleSheet("QLabel { color: #14C814; }");
        toolBarSafetyLabel->setText(tr("SAFE"));
    }

    changed = false;
}


void ToolBar::updateWaypointDistance(double distance)
{
    if (wpDistance != distance) changed = true;
    wpDistance = distance;
}

void ToolBar::updateCurrentWaypoint(quint16 id)
{
    if (wpId != id) changed = true;
    wpId = id;
}

void ToolBar::updateBatteryRemaining(UASInterface* uas, double voltage, double percent, int seconds)
{
    Q_UNUSED(uas);
    Q_UNUSED(seconds);
    if (batteryPercent != percent || batteryVoltage != voltage) changed = true;
    batteryPercent = percent;
    batteryVoltage = voltage;
}

void ToolBar::updateState(UASInterface* system, QString name, QString description)
{
    Q_UNUSED(system);
    Q_UNUSED(description);
    if (state != name) changed = true;
    state = name;
    /* important, immediately update */
    updateView();
}

void ToolBar::updateMode(int system, QString name, QString description)
{
    Q_UNUSED(system);
    Q_UNUSED(description);
    if (mode != name) changed = true;
    mode = name;
    /* important, immediately update */
    updateView();
}

void ToolBar::updateName(const QString& name)
{
    if (systemName != name)
    {
        changed = true;
    }
    systemName = name;
}

/**
 * The current system type is represented through the system icon.
 *
 * @param uas Source system, has to be the same as this->uas
 * @param systemType type ID, following the MAVLink system type conventions
 * @see http://pixhawk.ethz.ch/software/mavlink
 */
void ToolBar::setSystemType(UASInterface* uas, unsigned int systemType)
{
    Q_UNUSED(uas);
    QPixmap newPixmap;
    // Set matching icon
    switch (systemType) {
    case MAV_TYPE_GENERIC:
        newPixmap = QPixmap(":/files/images/mavs/generic.svg");
        break;
    case MAV_TYPE_FIXED_WING:
        newPixmap = QPixmap(":/files/images/mavs/fixed-wing.svg");
        break;
    case MAV_TYPE_QUADROTOR:
        newPixmap = QPixmap(":/files/images/mavs/quadrotor.svg");
        break;
    case MAV_TYPE_COAXIAL:
        newPixmap = QPixmap(":/files/images/mavs/coaxial.svg");
        break;
    case MAV_TYPE_HELICOPTER:
        newPixmap = QPixmap(":/files/images/mavs/helicopter.svg");
        break;
    case MAV_TYPE_ANTENNA_TRACKER:
        newPixmap = QPixmap(":/files/images/mavs/antenna-tracker.svg");
        break;
    case MAV_TYPE_GCS:
        newPixmap = QPixmap(":files/images/mavs/groundstation.svg");
        break;
    case MAV_TYPE_AIRSHIP:
        newPixmap = QPixmap(":files/images/mavs/airship.svg");
        break;
    case MAV_TYPE_FREE_BALLOON:
        newPixmap = QPixmap(":files/images/mavs/free-balloon.svg");
        break;
    case MAV_TYPE_ROCKET:
        newPixmap = QPixmap(":files/images/mavs/rocket.svg");
        break;
    case MAV_TYPE_GROUND_ROVER:
        newPixmap = QPixmap(":files/images/mavs/ground-rover.svg");
        break;
    case MAV_TYPE_SURFACE_BOAT:
        newPixmap = QPixmap(":files/images/mavs/surface-boat.svg");
        break;
    case MAV_TYPE_SUBMARINE:
        newPixmap = QPixmap(":files/images/mavs/submarine.svg");
        break;
    case MAV_TYPE_HEXAROTOR:
        newPixmap = QPixmap(":files/images/mavs/hexarotor.svg");
        break;
    case MAV_TYPE_OCTOROTOR:
        newPixmap = QPixmap(":files/images/mavs/octorotor.svg");
        break;
    case MAV_TYPE_TRICOPTER:
        newPixmap = QPixmap(":files/images/mavs/tricopter.svg");
        break;
    case MAV_TYPE_FLAPPING_WING:
        newPixmap = QPixmap(":files/images/mavs/flapping-wing.svg");
        break;
    case MAV_TYPE_KITE:
        newPixmap = QPixmap(":files/images/mavs/kite.svg");
        break;
    default:
        newPixmap = QPixmap(":/files/images/mavs/unknown.svg");
        break;
    }
    symbolLabel->setPixmap(newPixmap.scaledToHeight(34));
}

void ToolBar::receiveTextMessage(int uasid, int componentid, int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);
    Q_UNUSED(severity);
    text = text.trimmed();
    if (lastSystemMessage != text) changed = true;
    lastSystemMessage = text;
}

void ToolBar::addLink(LinkInterface* link)
{
    // XXX magic number
    if (LinkManager::instance()->getLinks().count() > 1) {
        currentLink = link;
//        connect(currentLink, SIGNAL(connected(bool)), this, SLOT(updateLinkState(bool)));
//        updateLinkState(link->isConnected());
    }
}

void ToolBar::removeLink(LinkInterface* link)
{
    if (link == currentLink) {
        currentLink = NULL;
        // XXX magic number
        if (LinkManager::instance()->getLinks().count() > 1) {
            currentLink = LinkManager::instance()->getLinks().last();
//            updateLinkState(currentLink->isConnected());
        }
//        else {
//            connectButton->setText(tr("New Link"));
//        }
    }
}

ToolBar::~ToolBar()
{
    if (toggleLoggingAction) toggleLoggingAction->deleteLater();
    if (logReplayAction) logReplayAction->deleteLater();
}

