#ifndef MAVLINKUASFACTORY_H
#define MAVLINKUASFACTORY_H

#include <QObject>

#include "common/mavlink.h"
#include "mavlinkprotocol.h"
#include "uasinterface.h"
#include "linkinterface.h"
#include "uas.h"

class MAVLinkUASFactory : public QObject
{
    Q_OBJECT
public:
    explicit MAVLinkUASFactory(QObject *parent = 0);

    /**  Create a new UAS object using MAVLink as protocol */
    static UASInterface* createUAS(MAVLinkProtocol* mavlink, LinkInterface* link, int sysid, mavlink_heartbeat_t* heartbeat, QObject* parent=NULL);

signals:

public slots:

};

#endif // MAVLINKUASFACTORY_H
