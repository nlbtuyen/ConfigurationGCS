#include "commconfigurationwindow.h"
#include "ui_commsettings.h"
#include "serialconfigurationwindow.h"
#include "seriallink.h"
#include "mavlinkprotocol.h"
#include "linkmanager.h"
#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QBoxLayout>
#include <QWidget>

CommConfigurationWindow::CommConfigurationWindow(LinkInterface* link, ProtocolInterface* protocol, QWidget *parent) : QWidget(NULL)
{
    Q_UNUSED(parent);
    this->link = link;

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    // Setup the user interface according to link type
    ui.setupUi(this);

    // add link types
    ui.linkType->addItem(tr("Serial"), QGC_LINK_SERIAL);
    ui.linkType->addItem(tr("UDP"), QGC_LINK_UDP);
    //ui.linkType->addItem(tr("Simulation"), QGC_LINK_SIMULATION);
    ui.linkType->setEditable(false);

    ui.connectionType->addItem("MAVLink", QGC_PROTOCOL_MAVLINK);

    ui.connectButton->setProperty("type", "toggle-ok-warn");
    ui.deleteButton->setProperty("type", "push-vital");

    // Create action to open this menu
    // Create configuration action for this link
    // Connect the current UAS
    action = new QAction(QIcon(":images/network-wireless.png"), "", this);
    LinkManager::instance()->add(link);
    action->setData(link->getId());
    action->setEnabled(true);
    action->setVisible(true);
    setLinkName(link->getName());
    connect(action, SIGNAL(triggered()), this, SLOT(show()));

    // Make sure that a change in the link name will be reflected in the UI
    connect(link, SIGNAL(nameChanged(QString)), this, SLOT(setLinkName(QString)));

    // Setup user actions and link notifications
    connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(setConnection()));
    connect(ui.closeButton, SIGNAL(clicked()), this->window(), SLOT(close()));
    connect(ui.deleteButton, SIGNAL(clicked()), this, SLOT(remove()));

    connect(this->link, SIGNAL(connected(bool)), this, SLOT(connectionState(bool)));


    // Fill in the current data
    if(this->link->isConnected()) ui.connectButton->setChecked(true);
    //connect(this->link, SIGNAL(connected(bool)), ui.connectButton, SLOT(setChecked(bool)));

    if(this->link->isConnected()) {
        ui.connectionStatusLabel->setText(tr("Connected"));

        // TODO Deactivate all settings to force user to manually disconnect first
    } else {
        ui.connectionStatusLabel->setText(tr("Disconnected"));
    }

    // TODO Move these calls to each link so that dynamic casts vanish

    // Open details pane for serial link if necessary
    SerialLink* serial = dynamic_cast<SerialLink*>(link);
    if(serial != 0) {
        QWidget* conf = new SerialConfigurationWindow(serial, this);
        ui.linkScrollArea->setWidget(conf);
        ui.linkGroupBox->setTitle(tr("Serial Link"));
        ui.linkType->setCurrentIndex(0);
    }
//    UDPLink* udp = dynamic_cast<UDPLink*>(link);
//    if (udp != 0) {
//        QWidget* conf = new QGCUDPLinkConfiguration(udp, this);
//        ui.linkScrollArea->setWidget(conf);
//        ui.linkGroupBox->setTitle(tr("UDP Link"));
//        ui.linkType->setCurrentIndex(1);
//    }
//    MAVLinkSimulationLink* sim = dynamic_cast<MAVLinkSimulationLink*>(link);
//    if (sim != 0) {
//        ui.linkType->setCurrentIndex(2);
//        ui.linkGroupBox->setTitle(tr("MAVLink Simulation Link"));
//}
    // Open details pane for MAVLink if necessary
    MAVLinkProtocol* mavlink = dynamic_cast<MAVLinkProtocol*>(protocol);
    if (mavlink != 0) {
//        QWidget* conf = new MAVLinkSettingsWidget(mavlink, this);
//        ui.protocolScrollArea->setWidget(conf);
//        ui.protocolGroupBox->setTitle(protocol->getName()+" (Global Settings)");
    } else {
        qDebug() << "Protocol is NOT MAVLink, can't open configuration window";
    }

    // Display the widget
    this->window()->setWindowTitle(tr("Settings for ") + this->link->getName());
    this->hide();
}

CommConfigurationWindow::~CommConfigurationWindow()
{
    saveSettings();
}

void CommConfigurationWindow::hideEvent(QHideEvent* event)
{
    saveSettings();
    QWidget::hideEvent(event);
}

void CommConfigurationWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    QWidget::closeEvent(event);
}

void CommConfigurationWindow::showEvent(QShowEvent* event)
{
    loadSettings();
    QWidget::showEvent(event);
}


void CommConfigurationWindow::saveSettings() {
    settings.beginGroup("COMM_CONFIG_WINDOW");
    settings.setValue("WINDOW_POSITION", this->window()->saveGeometry());
    settings.endGroup();
    settings.sync();
}

void CommConfigurationWindow::loadSettings() {
    settings.beginGroup("COMM_CONFIG_WINDOW");
    if (settings.contains("WINDOW_POSITION"))
        this->window()->restoreGeometry(settings.value("WINDOW_POSITION").toByteArray());
    settings.endGroup();

}

QAction* CommConfigurationWindow::getAction()
{
    return action;
}

void CommConfigurationWindow::setLinkType(int linktype)
{
    if(link->isConnected())
    {
        // close old configuration window
        this->window()->close();
    }
    else
    {
        // delete old configuration window
        this->remove();
    }

    LinkInterface *tmpLink(NULL);
    switch(linktype)
    {
        case 1:
            {
//                UDPLink *udp = new UDPLink();
//                tmpLink = udp;
                break;
            }
        case 2:
            {
//                MAVLinkSimulationLink *sim = new MAVLinkSimulationLink();
//                tmpLink = sim;
                break;
            }
        default:
        case 0:
            SerialLink *serial = new SerialLink();
            tmpLink = serial;
            MainWindow::instance()->addLink(tmpLink);

            break;
    }

    const int32_t& linkIndex(LinkManager::instance()->getLinks().indexOf(tmpLink));
    const int32_t& linkID(LinkManager::instance()->getLinks()[linkIndex]->getId());

    QList<QAction*> actions = MainWindow::instance()->listLinkMenuActions();
    foreach (QAction* act, actions)
    {
        if (act->data().toInt() == linkID)
        {
            act->trigger();
            break;
        }
    }

}

void CommConfigurationWindow::setProtocol(int protocol)
{
    qDebug() << "Changing to protocol" << protocol;
}

void CommConfigurationWindow::setConnection()
{
    if(!link->isConnected()) {
        ui.connectButton->setChecked(false);  // this will be set once actually connected
        link->connect();
    } else {
        ui.connectButton->setChecked(true);  // this will be set once actually connected
        link->disconnect();
    }
}

void CommConfigurationWindow::setLinkName(QString name)
{
    action->setText(tr("%1 Settings").arg(name));
    action->setStatusTip(tr("Adjust setting for link %1").arg(name));
    this->window()->setWindowTitle(tr("Settings for %1").arg(name));
}

void CommConfigurationWindow::remove()
{
    if(action) delete action; //delete action first since it has a pointer to link
    action=NULL;

    if(link) {
        link->disconnect(); //disconnect port, and also calls terminate() to stop the thread
        //if (link->isRunning()) link->terminate(); // terminate() the serial thread just in case it is still running
        link->wait(); // wait() until thread is stoped before deleting
        LinkManager::instance()->removeLink(link); //remove link from LinkManager list
        link->deleteLater();
    }
    link=NULL;

    this->window()->close();
    this->deleteLater();
}

void CommConfigurationWindow::connectionState(bool connect)
{
    ui.connectButton->setChecked(connect);
    if(connect) {
        ui.connectionStatusLabel->setText(tr("Connected"));
        ui.connectButton->setText(tr("Disconnect"));
    } else {
        ui.connectionStatusLabel->setText(tr("Disconnected"));
        ui.connectButton->setText(tr("Connect"));
    }
}
