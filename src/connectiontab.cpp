#include "connectiontab.h"
#include "ui_connectiontab.h"
#include "serialportsetting.h"
#include "seriallink.h"
#include "mavlinkprotocol.h"
#include "linkmanager.h"
#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QBoxLayout>
#include <QWidget>

ConnectionTab::ConnectionTab(LinkInterface* link, ProtocolInterface* protocol, QWidget *parent) : QWidget(NULL)
{
    Q_UNUSED(parent);
    this->link = link;

    ui.setupUi(this);

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

    SerialLink* serial = dynamic_cast<SerialLink*>(link);
    if(serial != 0) {
        QWidget* conf = new SerialPortSetting(serial, this);
        ui.linkScrollArea->setWidget(conf);
        ui.linkGroupBox->setTitle(tr("Serial Link"));
    }

    MAVLinkProtocol* mavlink = dynamic_cast<MAVLinkProtocol*>(protocol);
    if (mavlink == 0)
    {
        qDebug() << "Protocol is NOT MAVLink, can't open configuration window";
    }
}

ConnectionTab::~ConnectionTab()
{
    saveSettings();
}

void ConnectionTab::hideEvent(QHideEvent* event)
{
    saveSettings();
    QWidget::hideEvent(event);
}

void ConnectionTab::closeEvent(QCloseEvent* event)
{
    saveSettings();
    QWidget::closeEvent(event);
}

void ConnectionTab::showEvent(QShowEvent* event)
{
    loadSettings();
    QWidget::showEvent(event);
}


void ConnectionTab::saveSettings() {
    settings.beginGroup("COMM_CONFIG_WINDOW");
    settings.setValue("WINDOW_POSITION", this->window()->saveGeometry());
    settings.endGroup();
    settings.sync();
}

void ConnectionTab::loadSettings() {
    settings.beginGroup("COMM_CONFIG_WINDOW");
    if (settings.contains("WINDOW_POSITION"))
        this->window()->restoreGeometry(settings.value("WINDOW_POSITION").toByteArray());
    settings.endGroup();

}

QAction* ConnectionTab::getAction()
{
    return action;
}

void ConnectionTab::setLinkType(int linktype)
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

            SerialLink *serial = new SerialLink();
            tmpLink = serial;
            MainWindow::instance()->addLink(tmpLink);

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

void ConnectionTab::setProtocol(int protocol)
{
    qDebug() << "Changing to protocol" << protocol;
}

void ConnectionTab::setConnection()
{
    if(!link->isConnected()) {
        ui.connectButton->setChecked(false);  // this will be set once actually connected
        link->connect();
    } else {
        ui.connectButton->setChecked(true);  // this will be set once actually connected
        link->disconnect();
    }
}

void ConnectionTab::setLinkName(QString name)
{
    action->setText(tr("%1 Settings").arg(name));
    action->setStatusTip(tr("Adjust setting for link %1").arg(name));
    this->window()->setWindowTitle(tr("Settings for %1").arg(name));
}

void ConnectionTab::remove()
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

void ConnectionTab::connectionState(bool connect)
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


