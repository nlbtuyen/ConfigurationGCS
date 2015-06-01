#include "seriallink.h"
#include "linkmanager.h"
#include "qgc.h"
#include "settingsdialog.h"
#include "mainwindow.h"
#include <QDebug>

SerialLink::SerialLink(QString portname, int baudRate, bool hardwareFlowControl, bool parity,
                       int dataBits, int stopBits):
    // port(0),
    portOpenMode(QIODevice::ReadWrite),
    portProductId(0),
    bitsSentTotal(0),
    bitsSentShortTerm(0),
    bitsSentCurrent(0),
    bitsSentMax(0),
    bitsReceivedTotal(0),
    bitsReceivedShortTerm(0),
    bitsReceivedCurrent(0),
    bitsReceivedMax(0),
    connectionStartTime(0),
    ports(new QVector<QString>()),
    waitingToReconnect(0),
    m_reconnectDelayMs(0),
    m_linkLossExpected(false),
    m_stopp(false),
    mode_port(false),
    countRetry(0),
    maxLength(0),
    rows(0),
    cols(0),
    firstRead(0)
{
    port = new QSerialPort();
    portEnum = new QSerialPortInfo();
    portSettings = new SettingsDialog();
    getCurrentPorts();
    this->porthandle = portname.trimmed();
    if (!ports->contains(porthandle))
        porthandle = "";

    this->id = getNextLinkId();

    //    setBaudRate();
    //    setFlowType(QSerialPort::NoFlowControl);

    int fc = 0;
    int par = 0;

    setBaudRate(baudRate);
    setFlowType(fc);
    setParityType(par);
    setDataBitsType(dataBits);
    setStopBitsType(stopBits);
    //    port->setParity(QSerialPort::NoParity);
    //    port->setDataBits(QSerialPort::Data8);
    //    port->setFlowControl(QSerialPort::NoFlowControl);
    //    setTimeoutMillis(-1);

    //    name = this->porthandle.length() ? this->porthandle : tr("Serial Link ") + QString::number(getId());
    name = "com7";
    //    port->setPortName("com7");
    setName(name);
    QObject::connect(this, SIGNAL(portError()), this, SLOT(disconnect()));

}

SerialLink::~SerialLink()
{

}

bool SerialLink::connect()
{
    if (this->isRunning()) {
        this->disconnect();
        this->wait();
    }
    //    qDebug() << "go to connect()";

    //     reset the run stop flag
    m_stoppMutex.lock();
    m_stopp = false;
    m_stoppMutex.unlock();

//    waitingToReconnect = 0;
//    m_linkLossExpected = false;

    SettingsDialog::Settings p = portSettings->settings();
    this->port->setBaudRate(p.baudRate);
    this->port->setPortName(p.name);
    this->port->setDataBits(p.dataBits);
    this->port->setParity(p.parity);
    this->port->setStopBits(p.stopBits);
    this->port->setFlowControl(p.flowControl);

    //    this->port->setBaudRate(QSerialPort::Baud115200);
    //    this->port->setPortName("com7");
    //    this->port->setDataBits(QSerialPort::Data8);
    //    this->port->setParity(QSerialPort::NoParity);
    //    this->port->setStopBits(QSerialPort::OneStop);
    //    this->port->setFlowControl(QSerialPort::NoFlowControl);

    this->port->open(QSerialPort::ReadWrite);


    this->start(QThread::LowPriority);
    return this->isRunning();
}
void SerialLink::run()
{
    // Initialize the connection
    //    if (!hardwareConnect())
    //        return;

    //    qDebug() << "go to run()";
    while (!m_stopp)
    {

        this->readBytes();

        // Serial data isn't arriving that fast normally, this saves the thread
        // from consuming too much processing time
        msleep(SerialLink::poll_interval);
    }
}

QVector<QString> *SerialLink::getCurrentPorts()
{
    ports->clear();

    foreach (const QSerialPortInfo &p, portEnum->availablePorts())
    {
        if (p.portName().length())
        {
            ports->append(p.portName());
        }
        return this->ports;
    }
}

void SerialLink::readBytes() //@Leo
{

    const qint64 maxLength = 2048;
    char data[maxLength];
    qint64 numBytes = 0, rBytes = 0;

    dataMutex.lock();

    while ((numBytes = this->port->bytesAvailable())) {
        rBytes = numBytes;
        if(maxLength < rBytes) rBytes = maxLength;

        if (port->read(data, rBytes) == -1) {
            emit portError();

        }
        QByteArray b(data, rBytes);
        emit bytesReceived(this, b); //@Leo signal bytesReceive in LinkInterface
        bitsReceivedTotal += rBytes * 8;
    }

    dataMutex.unlock();
}

bool SerialLink::hardwareConnect()
{
    if (!isPortHandleValid()) {
        emit communicationError(this->getName(), tr("Failed to open serial port %1 because it no longer exists in the system.").arg(porthandle));
        return false;
    }

    if (!port) {
        port = new QSerialPort();
        if (!port) {
            emit communicationError(this->getName(), tr("Could not create serial port object."));
            return false;
        }
        QObject::connect(port, SIGNAL(aboutToClose()), this, SIGNAL(disconnected()));
        //QObject::connect(port, SIGNAL(readyRead()), this, SLOT(readBytes()), Qt::DirectConnection);
    }

    if (port->isOpen())
        port->close();

    QString err;
    SettingsDialog::Settings p = portSettings->settings();

    port->setPortName("com7");
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    //    if (!port->open(portOpenMode)) {
    //        err = tr("Failed to open serial port %1").arg(this->porthandle);
    //        if (port->lastError() != E_NO_ERROR)
    //            err = err % tr(" with error: %1 (%2)").arg(port->errorString()).arg(port->lastError());
    //        else
    //            err = err % tr(". It may already be in use, please check your connections.");
    //    }

    if (err.length()) {
        emit communicationError(this->getName(), err);
        //        qWarning() << err << port->portName() << port->baudRate() << "db:" << port->dataBits() \
        //                   << "p:" << port->parity() << "sb:" << port->stopBits() << "fc:" << port->flowControl();

        if (port->isOpen())
            port->close();
        port->deleteLater();
        port = NULL;

        return false;
    }

    //connectionStartTime = MG::TIME::getGroundTimeNow();
    //setUsbDeviceInfo();

    if(isConnected()) {
        emit connected();
        emit connected(true);
        //        writeSettings();
        qDebug() << "Connected Serial" << porthandle  << "with settings" \
                 << port->portName() << port->baudRate() << "db:" << port->dataBits() \
                 << "p:" << port->parity() << "sb:" << port->stopBits() << "fc:" << port->flowControl();
    } else
        return false;

    return true;
}
bool SerialLink::disconnect()
{
    if(this->isRunning() && !m_stopp) {
                m_stoppMutex.lock();
        this->m_stopp = true;
        //m_waitCond.wakeOne();
                m_stoppMutex.unlock();
        this->wait();
    }

    if (this->isConnected())
        port->close();

    if (port) {
        port->deleteLater();
        port = NULL;
    }

    //    portVendorId = 0;
    //    portProductId = 0;

    emit disconnected();
    emit connected(false);
    return !this->isConnected();
}
bool SerialLink::isConnected()
{
    if (port)
        return port->isOpen(); //isPortHandleValid() &&
    else
        return false;
}

bool SerialLink::isPortValid(const QString &pname)
{
    return getCurrentPorts()->contains(pname);
}

bool SerialLink::isPortHandleValid()
{
    return isPortValid(porthandle);

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
QString SerialLink::getPortName()
{
    return porthandle;
}
QString SerialLink::getName()
{
    return name;
}
int SerialLink::getBaudRate()
{
    return getNominalDataRate();
}

int SerialLink::getBaudRateType()
{
    return getNominalDataRate();
}

int SerialLink::getFlowType()
{
    return 0;
}

int SerialLink::getParityType()
{
    return 0;
}

int SerialLink::getDataBitsType()
{
    return 8;
}

int SerialLink::getStopBitsType()
{
    return 1;
}

qint64 SerialLink::getNominalDataRate()
{
    return 115200;
}

qint64 SerialLink::getTotalUpstream()
{
    QMutexLocker locker(&statisticsMutex);
    //    return bitsSentTotal / ((MG::TIME::getGroundTimeNow() - connectionStartTime) / 1000);
    return 0;
}

qint64 SerialLink::getCurrentUpstream()
{
    return 0; // TODO
}

qint64 SerialLink::getMaxUpstream()
{
    return 0; // TODO
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
    //    return bitsReceivedTotal / ((MG::TIME::getGroundTimeNow() - connectionStartTime) / 1000);
    return 0;
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
int SerialLink::getId()
{
    return id;
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
        //        loadSettings();
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
    bool accepted = false;
    //    BaudRateType br = (BaudRateType)(rate);
    //    if (br) {
    //        portSettings.BaudRate = br;
    //        if (port)
    //            port->setBaudRate(portSettings.BaudRate);
    //        accepted = true;
    //    }
    return true;
}

bool SerialLink::setTimeoutMillis(const long &ms)
{
    //    portSettings.Timeout_Millisec = ms;
    if (port)
        //        port->setTimeout(portSettings.Timeout_Millisec);
        return true;
}

bool SerialLink::setFlowType(int flow)
{
    //    bool accepted = false;
    //    if (flow >= (int)FLOW_OFF && flow <= (int)FLOW_XONXOFF) {
    //        portSettings.FlowControl = (FlowType)flow;
    //        if (port)
    //            port->setFlowControl(portSettings.FlowControl);
    //        accepted = true;
    //    }
    return true;
}

bool SerialLink::setParityType(int parity)
{
    //    bool accepted = false;
    //    if (parity >= (int)PAR_NONE && parity <= (int)PAR_SPACE) {
    //        portSettings.Parity = (ParityType)parity;
    //        if (port)
    //            port->setParity(portSettings.Parity);
    //        accepted = true;
    //    }
    return true;
}


bool SerialLink::setDataBitsType(int dataBits)
{
    //    bool accepted = false;
    //    if (dataBits >= (int)DATA_5 && dataBits <= (int)DATA_8) {
    //        portSettings.DataBits = (DataBitsType)dataBits;
    //        if (port)
    //            port->setDataBits(portSettings.DataBits);
    //        accepted = true;
    //    }
    return true;
}

bool SerialLink::setStopBitsType(int stopBits)
{
    //    bool accepted = false;
    //    if (stopBits == 1 || stopBits == 2) {
    //        portSettings.StopBits = stopBits == 1 ? STOP_1 : STOP_2;
    //        if (port)
    //            port->setStopBits(portSettings.StopBits);
    //        accepted = true;
    //    }
    return true;
}

void SerialLink::setEsc32Mode(bool mode) {
    mode_port = mode;
    if ( mode_port ) {
        this->rows = 0;
        this->cols = 0;
        firstRead = 1;
    }
}

void SerialLink::setReconnectDelayMs(const quint16 &ms)
{
    m_reconnectDelayMs = ms;
}

void SerialLink::linkLossExpected(const bool yes)
{
    m_linkLossExpected = yes;
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
        if (!m_linkLossExpected)
            emit communicationError(this->getName(), tr("Could not send data - error on link %1").arg(this->porthandle));
    }
}
QSerialPort * SerialLink::getPort() {
    return port;
}
bool SerialLink::validateConnection() {
    bool ok = this->isConnected();
    if (ok && (portOpenMode & QIODevice::ReadOnly) && !port->isReadable())
        ok = false;
    if (ok && (portOpenMode & QIODevice::WriteOnly) && !port->isWritable())
        ok = false;
    if(!ok) {
        emit portError();
        if (!m_linkLossExpected)
            emit communicationError(this->getName(), tr("Link %1 unexpectedly disconnected!").arg(this->porthandle));
        //        qWarning() << ok << port->lastError() << port->errorString();
        //this->disconnect();
        return false;
    }
    return true;
}

void SerialLink::deviceRemoved(const QSerialPortInfo &pi)
{

}

void SerialLink::deviceDiscovered(const QSerialPortInfo &pi)
{

}

bool SerialLink::getEsc32Mode() {
    return mode_port;
}

void SerialLink::readEsc32Tele(){
    //dataMutex.lock();
    qint64 numBytes = 0;

    if (!validateConnection())
        return;

    // Clear up the input Buffer
    if ( this->firstRead == 1) {
        while(true) {
            port->read(data,1);
            if (data[0] == 'A') {
                break;
            }
        }
        this->firstRead = 2;
    }
retryA:
    if (!port->isOpen() || !mode_port)
        return;

    while( true) {
        numBytes = port->bytesAvailable();
        if ( numBytes > 0)
            break;
        msleep(1);
        if (!port->isOpen() || !mode_port)
            break;
    }
    if ( this->firstRead == 0) {
        numBytes = port->read(data,1);
        if ( numBytes  >= 1 ) {
            if ( data[0] != 'A') {
                goto retryA;
            }
        }
        else if (numBytes <= 0){
            msleep(5);
            goto retryA;
        }
    }
    else {
        this->firstRead = 0;
    }

    while( true) {
        numBytes = port->bytesAvailable();
        if ( numBytes > 0)
            break;
        msleep(1);
        if (!port->isOpen() || !mode_port)
            break;
    }
    numBytes = port->read(data,1);
    if ( numBytes  == 1 ) {
        if ( data[0] != 'q') {
            goto retryA;
        }
    }
    else if (numBytes <= 0){
        msleep(5);
        goto retryA;
    }

    while( true) {
        numBytes = port->bytesAvailable();
        if ( numBytes > 0)
            break;
        msleep(1);
        if (!port->isOpen() || !mode_port)
            break;
    }
    numBytes = port->read(data,1);
    if ( numBytes  == 1 ) {
        if ( data[0] != 'T') {
            msleep(5);
            goto retryA;
        }
    }
    else if (numBytes <= 0){
        msleep(5);
        goto retryA;
    }

    while( true) {
        numBytes = port->bytesAvailable();
        if ( numBytes >= 2)
            break;
        msleep(1);
        if (!port->isOpen() || !mode_port)
            break;
    }
    port->read(data,2);
    rows = 0;
    cols = 0;
    rows = data[0];
    cols = data[1];
    int length_array = (((cols*rows)*sizeof(float))+2);
    if ( length_array > 300) {
        qDebug() << "bad col " << cols << " rows " << rows;
        goto retryA;
    }
    while( true) {
        numBytes = port->bytesAvailable();
        if ( numBytes >= length_array)
            break;
        msleep(1);
        if (!port->isOpen() || !mode_port)
            break;
    }
    //qDebug() << "avalible" << numBytes;
    numBytes = port->read(data,length_array);
    if (numBytes == length_array ){
        QByteArray b(data, numBytes);
        emit teleReceived(b, rows, cols);
    }

    goto retryA;

    //dataMutex.unlock();
}


