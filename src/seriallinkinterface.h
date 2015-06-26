#ifndef SERIALLINKINTERFACE
#define SERIALLINKINTERFACE

#include <QObject>
#include <QString>
#include <QVector>
#include "linkinterface.h"

class SerialLinkInterface : public LinkInterface
{
    Q_OBJECT

public:
    virtual QVector<QString>* getCurrentPorts() = 0;
    virtual QString getPortName() = 0;
    virtual bool isPortValid(const QString &pname) = 0;
    virtual int getBaudRate() = 0;
    virtual long getTimeoutMillis() = 0;

    virtual int getBaudRateType() = 0;
    virtual int getFlowType() = 0;
    virtual int getParityType() = 0;
    virtual int getDataBitsType() = 0;
    virtual int getStopBitsType() = 0;
    virtual quint16 reconnectDelayMs() const = 0;

public slots:
    virtual bool setPortName(QString portName) = 0;
    virtual bool setBaudRate(int rate) = 0;
    virtual bool setBaudRateString(const QString& rate) = 0;
    virtual bool setTimeoutMillis(const long &ms) = 0;
    virtual bool setFlowType(int flow) = 0;
    virtual bool setParityType(int parity) = 0;
    virtual bool setDataBitsType(int dataBits) = 0;
    virtual bool setStopBitsType(int stopBits) = 0;
    virtual void linkLossExpected(const bool yes) = 0;
    virtual void setReconnectDelayMs(const quint16 &ms) = 0;

//    virtual void loadSettings() = 0;
//    virtual void writeSettings() = 0;

};


#endif // SERIALLINKINTERFACE

