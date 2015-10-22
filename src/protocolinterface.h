#ifndef PROTOCOLINTERFACE
#define PROTOCOLINTERFACE

#include <QThread>
#include <QString>
#include <QByteArray>
//#include "common/common.h"
//#include "common/mavlink.h"
//#include "common/mavlink_types.h"
#include "linkinterface.h"

class ProtocolInterface : public QObject
{
    Q_OBJECT
public:
    //virtual ~ProtocolInterface() {};
    virtual QString getName() = 0;

public slots:
    virtual void receiveBytes(LinkInterface *link, QByteArray b) = 0;

signals:
    /**  Update the packet loss from one system */
    void receiveLossChanged(int uasId, float loss);

};
#endif // PROTOCOLINTERFACE

