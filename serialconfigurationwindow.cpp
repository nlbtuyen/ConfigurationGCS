
#include <QDebug>

#include "mg.h"
#include "serialconfigurationwindow.h"
#include "seriallinkinterface.h"
#include <QDir>
#include <QSettings>
#include <QFileInfoList>
#include <QMessageBox>

SerialConfigurationWindow::SerialConfigurationWindow(LinkInterface* link, QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags),
    userConfigured(false)
{
    SerialLinkInterface* serialLink = dynamic_cast<SerialLinkInterface*>(link);

    if(serialLink != 0)
    {
        serialLink->loadSettings();
        this->link = serialLink;

        // Setup the user interface according to link type
        ui.setupUi(this);

        // Create action to open this menu
        // Create configuration action for this link
        // Connect the current UAS
        action = new QAction(QIcon(":/files/images/devices/network-wireless.svg"), "", link);
        setLinkName(link->getName());

        setupPortList();

        // Set up baud rates
        QList<int> supportedBaudRates = MG::SERIAL::getBaudRates();
        ui.baudRate->clear();
        for (int i = 0; i < supportedBaudRates.size(); ++i) {
            ui.baudRate->addItem(QString::number(supportedBaudRates.at(i)), supportedBaudRates.at(i));
        }

        // Load current link config
        // wtf is this supposed to do?
        //ui.portName->setCurrentIndex(-1);

        connect(action, SIGNAL(triggered()), this, SLOT(configureCommunication()));

        // Make sure that a change in the link name will be reflected in the UI
        connect(link, SIGNAL(nameChanged(QString)), this, SLOT(setLinkName(QString)));

        // Connect the individual user interface inputs
        connect(ui.portName, SIGNAL(activated(QString)), this, SLOT(setPortName(QString)));
        connect(ui.portName, SIGNAL(editTextChanged(QString)), this, SLOT(setPortName(QString)));
        connect(ui.baudRate, SIGNAL(activated(QString)), this->link, SLOT(setBaudRateString(QString)));
        connect(ui.flowControl_none, SIGNAL(toggled(bool)), this, SLOT(setFlowControlNone(bool)));
        connect(ui.flowControl_hw, SIGNAL(toggled(bool)), this, SLOT(setFlowControlHw(bool)));
        connect(ui.flowControl_sw, SIGNAL(toggled(bool)), this, SLOT(setFlowControlSw(bool)));
        connect(ui.parNone, SIGNAL(toggled(bool)), this, SLOT(setParityNone(bool)));
        connect(ui.parOdd, SIGNAL(toggled(bool)), this, SLOT(setParityOdd(bool)));
        connect(ui.parEven, SIGNAL(toggled(bool)), this, SLOT(setParityEven(bool)));
        connect(ui.dataBitsCombo, SIGNAL(editTextChanged(QString)), this, SLOT(setDataBits(QString)));
        connect(ui.stopBitsCombo, SIGNAL(editTextChanged(QString)), this, SLOT(setStopBits(QString)));

        //connect(this->link, SIGNAL(connected(bool)), this, SLOT());
        ui.portName->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        ui.baudRate->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        switch(this->link->getParityType()) {
        case 3:
            ui.parOdd->setChecked(true);
            break;
        case 2:
            ui.parEven->setChecked(true);
            break;
        case 0:
        default:
            ui.parNone->setChecked(true);
            break;
        }

        switch(this->link->getFlowType()) {
        case 1:
            ui.flowControl_hw->setChecked(true);
            break;
        case 2:
            ui.flowControl_sw->setChecked(true);
            break;
        case 0:
        default:
            ui.flowControl_none->setChecked(true);
            break;
        }

        ui.baudRate->setCurrentIndex(ui.baudRate->findText(QString("%1").arg(this->link->getBaudRate())));

        ui.dataBitsCombo->setEditText(QString::number(this->link->getDataBits()));
        ui.stopBitsCombo->setEditText(QString::number(this->link->getStopBits()));

        portCheckTimer = new QTimer(this);
        portCheckTimer->setInterval(2000);
        connect(portCheckTimer, SIGNAL(timeout()), this, SLOT(setupPortList()));

        // Display the widget
        this->window()->setWindowTitle(tr("Serial Communication Settings"));
    }
    else
    {
        qDebug() << "Link is NOT a serial link, can't open configuration window";
    }
}

SerialConfigurationWindow::~SerialConfigurationWindow()
{

}

void SerialConfigurationWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    portCheckTimer->start();
}

void SerialConfigurationWindow::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    portCheckTimer->stop();
}

QAction* SerialConfigurationWindow::getAction()
{
    return action;
}

void SerialConfigurationWindow::configureCommunication()
{
    QString selected = ui.portName->currentText();
    setupPortList();
    ui.portName->setEditText(selected);
    this->show();
}

void SerialConfigurationWindow::setupPortList()
{
    if (!link)
        return;

    QString selected = ui.portName->currentText();

    // Get the ports available on this system
    QVector<QString>* ports = link->getCurrentPorts();
    QString txt;
    // mark any invalid items in the selector (eg. port was disconnected)
    for (int i = 0; i < ui.portName->count(); ++i) {
        txt = ui.portName->itemData(i).toString();
        if (!ports->contains(txt))
            txt += " [INVALID PORT]";
        ui.portName->setItemText(i, txt);
    }
    // add any new ports
    for (int i = 0; i < ports->size(); ++i) {
        if (ui.portName->findData(ports->at(i)) == -1)
            ui.portName->addItem(ports->at(i), ports->at(i));
    }

    if (!selected.length() && ui.portName->count())
        selected = ui.portName->itemText(0);

    selected = selected.split("-").first().remove(" ");

    if (!userConfigured && selected.length() && link->isPortValid(selected))
        setPortName(selected);

    // Add the ports in reverse order, because we prepend them to the list
//    for (int i = ports->size() - 1; i >= 0; --i)
//    {
//        // Prepend newly found port to the list
//        if (ui.portName->findText(ports->at(i)) == -1)
//        {
//            ui.portName->insertItem(0, ports->at(i));
//            if (!userConfigured) ui.portName->setEditText(ports->at(i));
//        }
//    }
//    ui.portName->setEditText(this->link->getPortName());
}

void SerialConfigurationWindow::portError(const QString &err) {
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("Error in port settings"));
    msgBox.setInformativeText(tr("Can't set %1").arg(err));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void SerialConfigurationWindow::setFlowControlNone(bool accept)
{
    if (accept) {
        if (!link->setFlowType(0))
            portError(tr("Flow Control None"));
    }
}
void SerialConfigurationWindow::setFlowControlHw(bool accept)
{
    if (accept) {
        if (!link->setFlowType(1))
            portError(tr("Flow Control Hardware"));
    }
}
void SerialConfigurationWindow::setFlowControlSw(bool accept)
{
    if (accept) {
        if (!link->setFlowType(2))
            portError(tr("Flow Control Software"));
    }
}

void SerialConfigurationWindow::setParityNone(bool accept)
{
    if (accept) {
        if (!link->setParityType(0))
            portError(tr("Parity Type None"));
    }
}

void SerialConfigurationWindow::setParityOdd(bool accept)
{
    if (accept) {
        if (!link->setParityType(3))
            portError(tr("Parity Type Odd"));
    }
}

void SerialConfigurationWindow::setParityEven(bool accept)
{
    if (accept) {
        if (!link->setParityType(2))
            portError(tr("Parity Type Even"));
    }
}

void SerialConfigurationWindow::setDataBits(QString bits)
{
    if (!link->setDataBitsType(bits.toInt()))
        portError(tr("Data Bits to %1").arg(bits));
}

void SerialConfigurationWindow::setStopBits(QString bits)
{
    if (!link->setStopBitsType(bits.toInt()))
        portError(tr("Stop Bits to %1").arg(bits));
}

void SerialConfigurationWindow::setPortName(QString port)
{
//#ifdef Q_OS_WIN
    port = port.split("-").first().remove(" ");
//#endif
//    port = port.remove(" ");

    if (link->isPortValid(port) && this->link->getPortName() != port) {
        if (link->setPortName(port))
            userConfigured = true;
        else
            portError(tr("Port to %1").arg(port));
    }
}

void SerialConfigurationWindow::setLinkName(QString name)
{
    Q_UNUSED(name);
    // FIXME
    action->setText(tr("Configure ") + link->getName());
    action->setStatusTip(tr("Configure ") + link->getName());
    setWindowTitle(tr("Configuration of ") + link->getName());
}

