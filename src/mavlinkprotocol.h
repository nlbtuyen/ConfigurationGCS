#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H
#include <QObject>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include "protocolinterface.h"
#include "linkinterface.h"
#include "common/mavlink.h"
#include "qgc.h"
#include "mavlinkdecoder.h"

class MAVLinkDecoder;

class MAVLinkProtocol : public ProtocolInterface
{
    Q_OBJECT

public:
    MAVLinkProtocol();
    ~MAVLinkProtocol();

    /**  Get the human-friendly name of this protocol */
    QString getName();
    /**  Get the system id of this application */
    int getSystemId();
    /**  Get the component id of this application */
    int getComponentId();
    /**  The auto heartbeat emission rate in Hertz */
    int getHeartbeatRate();
    /**  Get heartbeat state */
    bool heartbeatsEnabled() const {
        return m_heartbeatsEnabled;
    }
    /**  Get logging state */
    bool loggingEnabled() const {
        return m_loggingEnabled;
    }
    /**  Get protocol version check state */
    bool versionCheckEnabled() const {
        return m_enable_version_check;
    }
    /**  Get the multiplexing state */
    bool multiplexingEnabled() const {
        return m_multiplexingEnabled;
    }
    /**  Get the authentication state */
    bool getAuthEnabled() {
        return m_authEnabled;
    }
    /**  Get the protocol version */
    int getVersion() {
        return MAVLINK_VERSION;
    }
    /**  Get the auth key */
    QString getAuthKey() {
        return m_authKey;
    }
    /**  Get the name of the packet log file */
    QString getLogfileName();
    /**  Get state of parameter retransmission */
    bool paramGuardEnabled() {
        return m_paramGuardEnabled;
    }
    /**  Get parameter read timeout */
    int getParamRetransmissionTimeout() {
        return m_paramRetransmissionTimeout;
    }
    /**  Get parameter write timeout */
    int getParamRewriteTimeout() {
        return m_paramRewriteTimeout;
    }
    /**  Get state of action retransmission */
    bool actionGuardEnabled() {
        return m_actionGuardEnabled;
    }
    /**  Get parameter read timeout */
    int getActionRetransmissionTimeout() {
        return m_actionRetransmissionTimeout;
    }

public slots:
    /**  Receive bytes from a communication interface */
    void receiveBytes(LinkInterface* link, QByteArray b);
    /**  Send MAVLink message through serial interface */
    void sendMessage(mavlink_message_t message);
    /**  Send MAVLink message */
    void sendMessage(LinkInterface* link, mavlink_message_t message);
    /**  Send MAVLink message with correct system / component ID */
    void sendMessage(LinkInterface* link, mavlink_message_t message, quint8 systemid, quint8 componentid);
    /**  Set the rate at which heartbeats are emitted */
    void setHeartbeatRate(int rate);
    /**  Set the system id of this application */
    void setSystemId(int id);

    /**  Enable / disable the heartbeat emission */
    void enableHeartbeats(bool enabled);

    /**  Enable/disable binary packet logging */
    void enableLogging(bool enabled);

    /**  Enabled/disable packet multiplexing */
    void enableMultiplexing(bool enabled);

    /**  Enable / disable parameter retransmission */
    void enableParamGuard(bool enabled);

    /**  Enable / disable action retransmission */
    void enableActionGuard(bool enabled);

    /**  Set parameter read timeout */
    void setParamRetransmissionTimeout(int ms);

    /**  Set parameter write timeout */
    void setParamRewriteTimeout(int ms);

    /**  Set parameter read timeout */
    void setActionRetransmissionTimeout(int ms);

    /**  Set log file name */
    void setLogfileName(const QString& filename);

    /**  Enable / disable version check */
    void enableVersionCheck(bool enabled);

    /**  Enable / disable authentication */
    void enableAuth(bool enable);

    /**  Set authentication token */
    void setAuthKey(QString key) {
        m_authKey = key;
    }

    /**  Send an extra heartbeat to all connected units */
    void sendHeartbeat();

    /**  Load protocol settings */
    void loadSettings();
    /**  Store protocol settings */
    void storeSettings();

protected:
    QTimer* heartbeatTimer;    ///< Timer to emit heartbeats
    int heartbeatRate;         ///< Heartbeat rate, controls the timer interval
    bool m_heartbeatsEnabled;  ///< Enabled/disable heartbeat emission
    bool m_multiplexingEnabled; ///< Enable/disable packet multiplexing
    bool m_authEnabled;        ///< Enable authentication token broadcast
    QString m_authKey;         ///< Authentication key
    bool m_loggingEnabled;     ///< Enable/disable packet logging
    QFile* m_logfile;           ///< Logfile
    bool m_enable_version_check; ///< Enable checking of version match of MAV and QGC
    int m_paramRetransmissionTimeout; ///< Timeout for parameter retransmission
    int m_paramRewriteTimeout;    ///< Timeout for sending re-write request
    bool m_paramGuardEnabled;       ///< Parameter retransmission/rewrite enabled
    bool m_actionGuardEnabled;       ///< Action request retransmission enabled
    int m_actionRetransmissionTimeout; ///< Timeout for parameter retransmission
    QMutex receiveMutex;       ///< Mutex to protect receiveBytes function
    int lastIndex[256][256];	///< Store the last received sequence ID for each system/componenet pair
    int totalReceiveCounter;
    int totalLossCounter;
    int currReceiveCounter;
    int currLossCounter;
    bool versionMismatchIgnore;
    int systemId;

signals:
    /**  Message received and directly copied via signal */
    void messageReceived(LinkInterface* link, mavlink_message_t message);
    /**  Emitted if heartbeat emission mode is changed */
    void heartbeatChanged(bool heartbeats);
    /**  Emitted if logging is started / stopped */
    void loggingChanged(bool enabled);
    /**  Emitted if multiplexing is started / stopped */
    void multiplexingChanged(bool enabled);
    /**  Emitted if authentication support is enabled / disabled */
    void authKeyChanged(QString key);
    /**  Authentication changed */
    void authChanged(bool enabled);
    /**  Emitted if version check is enabled / disabled */
    void versionCheckChanged(bool enabled);
    /**  Emitted if a message from the protocol should reach the user */
    void protocolStatusMessage(const QString& title, const QString& message);
    /**  Emitted if a new system ID was set */
    void systemIdChanged(int systemId);
    /**  Emitted if param guard status changed */
    void paramGuardChanged(bool enabled);
    /**  Emitted if param read timeout changed */
    void paramRetransmissionTimeoutChanged(int ms);
    /**  Emitted if param write timeout changed */
    void paramRewriteTimeoutChanged(int ms);
    /**  Emitted if action guard status changed */
    void actionGuardChanged(bool enabled);
    /**  Emitted if actiion request timeout changed */
    void actionRetransmissionTimeoutChanged(int ms);
};

#endif // MAVLINKPROTOCOL_H
