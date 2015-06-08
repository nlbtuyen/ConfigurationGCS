#include "seriallink.h"
#include "linkmanager.h"
#include "qgc.h"
#include "settingsdialog.h"
#include "mainwindow.h"
#include "mg.h"

#include <QDebug>
#include <QSettings>
#include <QMutexLocker>
#include <QSerialPortInfo>

SerialLink::SerialLink(QString portname, int baudRate, bool hardwareFlowControl, bool parity,
                       int dataBits, int stopBits):
    port(0),
    portSettings(PortSettings()),
    ports(new QVector<QString>()),
    bitsSentTotal(0),
    bitsSentShortTerm(0),
    bitsSentCurrent(0),
    bitsSentMax(0),
    bitsReceivedTotal(0),
    bitsReceivedShortTerm(0),
    bitsReceivedCurrent(0),
    bitsReceivedMax(0),
    connectionStartTime(0),
    m_stopp(false),
    mode_port(false),
    countRetry(0),
    maxLength(0),
    rows(0),
    cols(0),
    firstRead(0)
{
    //    port = new QSerialPort();
    //    portEnum = new QSerialPortInfo();
    //    getCurrentPorts();

    //Setup setting
    this->porthandle = portname.trimmed();
    QVector<QString> *cp = getCurrentPorts();
    if ((this->porthandle == "" || !cp->contains(porthandle)) && cp->size() > 0)
        this->porthandle = getCurrentPorts()->first().trimmed();


    // Set unique ID and add link to the list of links
    this->id = getNextLinkId();

    int par = parity ? (int)QSerialPort::EvenParity : (int)QSerialPort::NoParity;
    int fc = hardwareFlowControl ? (int)QSerialPort::HardwareControl : (int)QSerialPort::NoFlowControl;

    setBaudRate(baudRate);
    setFlowType(fc);
    setParityType(par);
    setDataBitsType(dataBits);
    setStopBitsType(stopBits);


    name = this->porthandle.length() ? this->porthandle : tr("Serial Link ") + QString::number(getId());

    if (name == this->porthandle || name == "")
        loadSettings();
}

SerialLink::~SerialLink()
{
    this->disconnect();
    if(port)
        delete port;
    if(ports)
        delete ports;
    port=NULL;
    ports=NULL;

}

QVector<QString> *SerialLink::getCurrentPorts()
{
    ports->clear();
    foreach (const QSerialPortInfo &p, QSerialPortInfo::availablePorts()) {
        if (p.isValid())
            ports->append(p.portName()  + " - " + p.description());

        qDebug() << p.portName()
                 << p.description()
                 << p.manufacturer()
                 << p.serialNumber()
                 << p.systemLocation()
                 << QString::number(p.vendorIdentifier(), 16)
                 << QString::number(p.productIdentifier(), 16)
                 << p.isBusy() << p.isNull() << p.isValid();
    }
    return this->ports;
}

void SerialLink::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    if (settings.contains("SERIALLINK_COMM_PORT"))
    {
        setPortName(settings.value("SERIALLINK_COMM_PORT").toString());
        setBaudRate(settings.value("SERIALLINK_COMM_BAUD").toInt());
        setParityType(settings.value("SERIALLINK_COMM_PARITY").toInt());
        setStopBitsType(settings.value("SERIALLINK_COMM_STOPBITS").toInt());
        setDataBitsType(settings.value("SERIALLINK_COMM_DATABITS").toInt());
        setFlowType(settings.value("SERIALLINK_COMM_FLOW_CONTROL").toInt());
    }
}

void SerialLink::writeSettings()
{
    // Store settings
    QSettings settings;
    settings.setValue("SERIALLINK_COMM_PORT", getPortName());
    settings.setValue("SERIALLINK_COMM_BAUD", getBaudRateType());
    settings.setValue("SERIALLINK_COMM_PARITY", getParityType());
    settings.setValue("SERIALLINK_COMM_STOPBITS", getStopBits());
    settings.setValue("SERIALLINK_COMM_DATABITS", getDataBits());
    settings.setValue("SERIALLINK_COMM_FLOW_CONTROL", getFlowType());
    settings.sync();
}


//Run the thread
void SerialLink::run()
{
    // Initialize the connection
    if (!hardwareConnect())
        return;

    while (!m_stopp)
    {
        if (!this->validateConnection()) {
            m_stoppMutex.lock();
            this->m_stopp = true;
            m_stoppMutex.unlock();
            break;
        }

        port->waitForReadyRead(SerialLink::readywait_interval);
        this->readBytes();

        // Serial data isn't arriving that fast normally, this saves the thread
        // from consuming too much processing time
        msleep(SerialLink::poll_interval);
    }
}

bool SerialLink::validateConnection() {
    bool ok = this->isConnected(); // && (!port->error() || port->error() == QSerialPort::TimeoutError)
    if (ok && (portSettings.openMode & QSerialPort::ReadOnly) && !port->isReadable())
        ok = false;
    if (ok && (portSettings.openMode & QSerialPort::WriteOnly) && !port->isWritable())
        ok = false;
    if(!ok) {
        emit portError();
        emit communicationError(this->getName(), tr("Link %1 unexpectedly disconnected!").arg(this->porthandle));
        qWarning() << __FILE__ << __LINE__ << ok << port->error() << port->errorString();
        //this->disconnect();
        return false;
    }
    return true;
}


void SerialLink::writeBytes(const char* data, qint64 size)
{
    if (!validateConnection())
        return;
    int b = port->write(data, size);

    if (b > 0) {
        // Increase write counter
        bitsSentTotal += b * 8;
        //qDebug() << "Serial link " << this->getName() << "transmitted" << b << "bytes:";
    } else if (b == -1) {
        emit portError();
        emit communicationError(this->getName(), tr("Could not send data - error on link %1: %2").arg(this->getName()).arg(port->errorString()));
    }
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/

void SerialLink::readBytes() //@Leo
{
    if (!validateConnection())
        return;
    const qint64 maxLength = 2048;
    char data[maxLength];
    qint64 numBytes = 0, rBytes = 0; //port->bytesAvailable();

    dataMutex.lock();
//    qDebug() << "pp" << port->bytesAvailable();
    while( numBytes = port->bytesAvailable()) {
        rBytes = numBytes;
        if(maxLength < rBytes) rBytes = maxLength;

        if (port->read(data, rBytes) == -1) { // -1 result means error
            emit portError();
            emit communicationError(this->getName(), tr("Could not read data - link %1 is disconnected!").arg(this->getName()));
            return;
        }

        QByteArray b(data, rBytes);
        emit bytesReceived(this, b);
        bitsReceivedTotal += rBytes * 8;

        for (int i=0; i<rBytes; i++){
            unsigned int v=data[i];
            fprintf(stderr,"%02x ", v);
        } fprintf(stderr,"\n");
    }
    dataMutex.unlock();
}

bool SerialLink::disconnect()
{
    if(this->isRunning() && !m_stopp) {
        m_stoppMutex.lock();
        this->m_stopp = true;
        //m_waitCond.wakeOne();
        m_stoppMutex.unlock();
        // w/out the following if(), when calling disconnect() from run() we can get a warning about thread waiting on itself
        //if (currentThreadId() != QThread::currentThreadId())
        this->wait();
    }

    if (this->isConnected())
        port->close();

    // delete (invalid) port. not sure this is necessary.
    if (port && !isPortHandleValid()) { //
        port->deleteLater();
        port = NULL;
    }

    emit disconnected();
    emit connected(false);
    return !this->isConnected();
}

bool SerialLink::connect()
{
    if (this->isRunning()) {
        this->disconnect();
        this->wait();
    }

    // reset the run stop flag
    m_stoppMutex.lock();
    m_stopp = false;
    m_stoppMutex.unlock();

    this->start(QThread::LowPriority);
    return this->isRunning();
}

bool SerialLink::hardwareConnect()
{
    QString err;

    if (!port) {
        port = new QSerialPort(0);
        port->setSettingsRestoredOnClose(false);
        QObject::connect(port, SIGNAL(aboutToClose()), this, SIGNAL(disconnected()));
        QObject::connect(port, SIGNAL(readyRead()), this, SLOT(readBytes()), Qt::DirectConnection);
        QObject::connect(this, SIGNAL(portError()), this, SLOT(disconnect()));
        // connecting to error() signal is the "proper" way to detect disconnect errors, but doesn't work with AQ native USB
        //        QObject::connect(port, SIGNAL(error(QSerialPort::SerialPortError)), this,
        //                         SLOT(handleError(QSerialPort::SerialPortError)), Qt::DirectConnection);
    }

    if (!port) {
        emit communicationError(this->getName(), tr("Could not create serial port object."));
        return false;
    }

    if(port->isOpen())
        port->close();

    if (!isPortHandleValid())
        err = tr("Failed to open serial port %1 because it no longer exists in the system.").arg(this->porthandle);
    else {
        QSerialPortInfo pi(this->porthandle);
        port->setPort(pi);

        if (!port->setBaudRate(portSettings.baudRate))
            err = tr("Failed to set Baud Rate to %1 with error: %2").arg((quint64)portSettings.baudRate).arg(port->errorString());

        else if (!port->setDataBits(portSettings.dataBits))
            err = tr("Failed to set Data Bits to %1 with error: %2").arg((int)portSettings.dataBits).arg(port->errorString());

        else if (!port->setParity(portSettings.parity))
            err = tr("Failed to set Parity to %1 with error: %2").arg((int)portSettings.parity).arg(port->errorString());

        else if (!port->setStopBits(portSettings.stopBits))
            err = tr("Failed to set Stop Bits to %1 with error: %2").arg((int)portSettings.stopBits).arg(port->errorString());

        else if (!port->setFlowControl(portSettings.flowControl))
            err = tr("Failed to set Flow Control to %1 with error: %2").arg((int)portSettings.flowControl).arg(port->errorString());

        else if (!port->open(portSettings.openMode))
            err = tr("Failed to open serial port %1 with error: %2 (%3)").arg(this->porthandle).arg(port->errorString()).arg(port->error());
    }

    if (err.length()) {
        emit communicationError(this->getName(), err);
        qWarning() << __FILE__ << __LINE__ << err << port->portName() << port->baudRate() << "db:" << port->dataBits() \
                   << "p:" << port->parity() << "sb:" << port->stopBits() << "fc:" << port->flowControl();
        if (port->isOpen())
            port->close();
        port->deleteLater();
        port = NULL;
        return false;
    }

    connectionStartTime = MG::TIME::getGroundTimeNow();

    if(isConnected()) {
        emit connected();
        emit connected(true);
        qDebug() << __FILE__ << __LINE__  << "Connected Serial" << porthandle  << "with settings" \
                 << port->portName() << port->baudRate() << "db:" << port->dataBits() \
                 << "p:" << port->parity() << "sb:" << port->stopBits() << "fc:" << port->flowControl();
    } else
        return false;

    writeSettings();

    return true;

    //    port->setPortName(porthandle);
    //    port->setBaudRate(portSettings.BaudRate);
    //    port->setDataBits(portSettings.DataBits);
    //    port->setParity(portSettings.Parity);
    //    port->setStopBits(portSettings.StopBits);
    //    port->setFlowControl(portSettings.FlowControl);
}

bool SerialLink::isPortValid(const QString &pname)
{
    QSerialPortInfo pi(pname);
    return pi.isValid();
}

bool SerialLink::isPortHandleValid()
{
    return isPortValid(porthandle);

}

bool SerialLink::isConnected()
{
    if (port)
    {
        port->open(QIODevice::ReadWrite);
        qDebug() <<  port->isOpen();
        return isPortHandleValid() && port->isOpen();
    }
    else
        return false;
}

// currently unused
qint64 SerialLink::bytesAvailable()
{
    if (port) {
        return port->bytesAvailable();
    } else {
        return 0;
    }
}

void SerialLink::handleError(QSerialPort::SerialPortError error)
{
    //qDebug() << __FILE__ << __LINE__ << error << port->error() << port->errorString();
    if (error == QSerialPort::ResourceError) {
        emit communicationError(this->getName(), tr("Link %1 unexpectedly disconnected with error: %2").arg(this->getName()).arg(port->errorString()));
        this->disconnect();
    }
}

bool SerialLink::setPortName(QString portName)
{
    portName = portName.trimmed();
    if(this->porthandle != portName && this->isPortValid(portName)) {
        if (isConnected())
            this->disconnect();
        if (isRunning())
            this->wait();

        this->porthandle = portName;
        setName(tr("serial port ") + portName);
    }
    return true;
}


void SerialLink::setName(QString name)
{
    if (this->name != name) {
        this->name = name;
        emit nameChanged(this->name);
    }
}

// doesn't seem to be used anywhere
bool SerialLink::setBaudRateString(const QString& rate)
{
    bool ok;
    int intrate = rate.toInt(&ok);
    if (!ok) return false;
    return setBaudRate(intrate);
}

bool SerialLink::setBaudRate(int rate)
{
    bool accepted = false; // This is changed if none of the data rates matches
    bool reconnect = isConnected();
    if (reconnect)
        this->disconnect();

    BaudRate br = (BaudRate)(rate);
    if (br) {
        portSettings.baudRate = br;
        if(reconnect)
            this->connect();
        accepted = true;
    }

    return accepted;
}

bool SerialLink::setFlowType(int flow)
{
    bool accepted = false;
    bool reconnect = isConnected();
    if (reconnect)
        this->disconnect();

    if (flow >= (int)QSerialPort::NoFlowControl && flow <= (int)QSerialPort::SoftwareControl) {
        portSettings.flowControl = (QSerialPort::FlowControl)flow;
        if (reconnect)
            this->connect();
        accepted = true;
    }

    return accepted;
}

bool SerialLink::setParityType(int parity)
{
    bool accepted = false;
    bool reconnect = isConnected();
    if (reconnect)
        this->disconnect();

    if (parity >= (int)QSerialPort::NoParity && parity <= (int)QSerialPort::MarkParity) {
        portSettings.parity = (QSerialPort::Parity)parity;
        if (reconnect)
            this->connect();
        accepted = true;
    }

    return accepted;
}


bool SerialLink::setDataBitsType(int dataBits)
{
    bool accepted = false;
    bool reconnect = isConnected();
    if (reconnect)
        this->disconnect();

    if (dataBits >= (int)QSerialPort::Data5 && dataBits <= (int)QSerialPort::Data8) {
        portSettings.dataBits = (QSerialPort::DataBits)dataBits;

        if(reconnect)
            this->connect();
        accepted = true;
    }

    return accepted;
}

bool SerialLink::setStopBitsType(int stopBits)
{
    bool accepted = false;
    bool reconnect = isConnected();
    if (reconnect)
        this->disconnect();

    if (stopBits >= (int)QSerialPort::OneStop && stopBits <= (int)QSerialPort::TwoStop) {
        portSettings.stopBits = (QSerialPort::StopBits)stopBits;

        if(reconnect)
            this->connect();
        accepted = true;
    }

    //if(reconnect) connect();
    return accepted;
}

QSerialPort * SerialLink::getPort() {
    return port;
}


int SerialLink::getId()
{
    return id;
}

QString SerialLink::getName()
{
    return name;
}

qint64 SerialLink::getNominalDataRate()
{
    return (qint64)portSettings.baudRate;
}

qint64 SerialLink::getTotalUpstream()
{
    QMutexLocker locker(&statisticsMutex);
    return bitsSentTotal / ((MG::TIME::getGroundTimeNow() - connectionStartTime) / 1000);
}

qint64 SerialLink::getCurrentUpstream()
{
    return 0;
}

qint64 SerialLink::getMaxUpstream()
{
    return 0;
}

qint64 SerialLink::getBitsSent()
{
    return bitsSentTotal;
}

qint64 SerialLink::getBitsReceived()
{
    return bitsReceivedTotal;
}

qint64 SerialLink::getTotalDownstream()
{
    QMutexLocker locker(&statisticsMutex);
    return bitsReceivedTotal / ((MG::TIME::getGroundTimeNow() - connectionStartTime) / 1000);
}


qint64 SerialLink::getCurrentDownstream()
{
    return 0; // TODO
}

qint64 SerialLink::getMaxDownstream()
{
    return 0; // TODO
}
bool SerialLink::isFullDuplex()
{
    /* Serial connections are always half duplex */
    return false;
}

int SerialLink::getLinkQuality()
{
    /* This feature is not supported with this interface */
    return -1;
}


QString SerialLink::getPortName()
{
    return porthandle;
}

int SerialLink::getBaudRate()
{
    return getNominalDataRate();
}

int SerialLink::getBaudRateType()
{
    return (quint64)portSettings.baudRate;
}

int SerialLink::getFlowType()
{
    return (int)portSettings.flowControl;
}

int SerialLink::getParityType()
{
    return (int) portSettings.parity;
}

int SerialLink::getDataBits()
{
    return (int) portSettings.dataBits;
}

int SerialLink::getStopBits()
{
    return (int)portSettings.stopBits;
}



