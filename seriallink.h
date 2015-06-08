#ifndef SERIALLINK_H
#define SERIALLINK_H
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QSerialPort>

#include <QSerialPortInfo>

#include "seriallinkinterface.h"

#define SERIAL_AQUSB_VENDOR_ID  1155
#define SERIAL_AQUSB_PRODUCT_ID  22352
#define SERIAL_IS_BUGGY_CP210x  (portVendorId == 4292 && portProductId == 60000)
#define SERIAL_POLL_INTERVAL 9

class SerialLink : public SerialLinkInterface
{
    Q_OBJECT
    //Q_INTERFACES(SerialLinkInterface:LinkInterface)

public:
    enum BaudRate {
        Baud1200 = 1200,
        Baud2400 = 2400,
        Baud4800 = 4800,
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200,
        Baud230400 = 230400,
        Baud460800 = 460800,
        Baud500000 = 500000,
        Baud576000 = 576000,
        Baud921600 = 921600
    };

    struct PortSettings
    {
        QString portName;
        BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
        QIODevice::OpenMode openMode;
        //long timeout_millis;

        PortSettings() :
            portName(QString()),
            baudRate(Baud57600),
            dataBits(QSerialPort::Data8),
            parity(QSerialPort::NoParity),
            stopBits(QSerialPort::OneStop),
            flowControl(QSerialPort::NoFlowControl),
            openMode(QIODevice::ReadWrite) {}
    };

    SerialLink(QString portname = "",
               int baudrate=115200,
               bool flow=false,
               bool parity=false,
               int dataBits=8,
               int stopBits=1);
    ~SerialLink();

    static const int reconnect_wait_timeout = 10 * 1000;  ///< ms to wait for an automatic reconnection attempt
    static const int poll_interval = SERIAL_POLL_INTERVAL;  ///< ms to sleep between run() loops, defined in configuration.h
    static const int readywait_interval = 1000;             ///< ms to wait for readyRead signal within run() loop (blocks this thread)
    /** @brief Get a list of the currently available ports */
    QVector<QString>* getCurrentPorts();

    bool isConnected();
    bool isPortValid(const QString &pname);
    bool isPortHandleValid();
    qint64 bytesAvailable();

    /**
     * @brief The port handle
     */
    QString getPortName();
    /**
     * @brief The human readable port name
     */
    QString getName();
    int getBaudRate();
    int getDataBits();
    int getStopBits();

    // ENUM values
    int getBaudRateType();
    int getFlowType();
    int getParityType();


    /* Extensive statistics for scientific purposes */
    qint64 getNominalDataRate();
    qint64 getTotalUpstream();
    qint64 getCurrentUpstream();
    qint64 getMaxUpstream();
    qint64 getTotalDownstream();
    qint64 getCurrentDownstream();
    qint64 getMaxDownstream();
    qint64 getBitsSent();
    qint64 getBitsReceived();

    void loadSettings();
    void writeSettings();

    void run();

    int getLinkQuality();
    bool isFullDuplex();
    int getId();
    //unsigned char read();

public slots:
    bool setPortName(QString portName);
    bool setBaudRate(int rate);

    // Set string rate
    bool setBaudRateString(const QString& rate);

    // Set ENUM values
    bool setFlowType(int flow);
    bool setParityType(int parity);
    bool setDataBitsType(int dataBits);
    bool setStopBitsType(int stopBits);

    void readBytes();
    QSerialPort *getPort();
    /**
     * @brief Write a number of bytes to the interface.
     *
     * @param data Pointer to the data byte array
     * @param size The size of the bytes array
     **/
    void writeBytes(const char* data, qint64 length);
    bool connect();
    bool disconnect();

protected slots:
    bool validateConnection();
    void handleError(QSerialPort::SerialPortError error);

protected:
    QSerialPort * port;
    PortSettings portSettings;
    QString porthandle;
    QString name;
    int timeout;
    int id;

    quint64 bitsSentTotal;
    quint64 bitsSentShortTerm;
    quint64 bitsSentCurrent;
    quint64 bitsSentMax;
    quint64 bitsReceivedTotal;
    quint64 bitsReceivedShortTerm;
    quint64 bitsReceivedCurrent;
    quint64 bitsReceivedMax;
    quint64 connectionStartTime;
    QMutex statisticsMutex;
    QMutex dataMutex;
    QVector<QString>* ports;

private:
    volatile bool m_stopp;
    QMutex m_stoppMutex;

    void setName(QString name);
    bool hardwareConnect();
    bool mode_port;
    int countRetry;
    int maxLength;
    char data[4096];
    int rows;
    int cols;
    int firstRead;


signals:
    void aboutToCloseFlag();
    void portError();

};
#endif // SERIALLINK_H
