#ifndef _LINKINTERFACE_H_
#define _LINKINTERFACE_H_

#include <QThread>

class LinkInterface : public QThread
{
    Q_OBJECT
public:

    enum LinkInterfaceTypes {
        LINK_INTERFACE_TYPE_NONE,
        LINK_INTERFACE_TYPE_SIMULATION,
        LINK_INTERFACE_TYPE_SERIAL,
        LINK_INTERFACE_TYPE_UDP,
        LINK_INTERFACE_TYPE_ENUM_END
    };

    LinkInterface(QObject* parent = 0) : QThread(parent) {}
    virtual ~LinkInterface() { emit this->deleteLink(this); }

    /* Connection management */

    /**
     *  Get the ID of this link
     *
     * The ID is an unsigned integer, starting at 0
     * @return ID of this link
     **/
    virtual int getId() = 0;

    /**
     *  Get the human readable name of this link
     */
    virtual QString getName() = 0;

    /**
     *  Get the port name, eg. COM port for serial or host:port for UDP or file name for Sim link
     */
    virtual QString getPortName() = 0;

    /**
     *  Get the LinkInterfaceTypes enum value of this link
     */
    virtual int getLinkType() = 0;

    /**
     *  Determine the connection status
     *
     * @return True if the connection is established, false otherwise
     **/
    virtual bool isConnected() = 0;

    /* Connection characteristics */

    /**
     *  Get the nominal data rate of the interface.
     *
     * The nominal data rate is the theoretical maximum data rate of the
     * interface. For 100Base-T Ethernet this would be 100 Mbit/s (100'000'000
     * Bit/s, NOT 104'857'600 Bit/s).
     *
     * @return The nominal data rate of the interface in bit per second, 0 if unknown
     * @see getLongTermDataRate() For the mean data rate
     * @see getShortTermDataRate() For a the mean data rate of the last seconds
     * @see getCurrentDataRate() For the data rate of the last transferred chunk
     * @see getMaxDataRate() For the maximum data rate
     **/
    virtual qint64 getNominalDataRate() = 0;

    /**
     *  Full duplex support of this interface.
     *
     * This method returns true if the interface supports full duplex, which implies
     * the full datarate when sending and receiving data simultaneously.
     *
     * @return True if the interface supports full duplex, false otherwise
     **/
    virtual bool isFullDuplex() = 0;

    /**
     *  Get the link quality.
     *
     * The link quality is reported as percent, on a scale from 0 to 100% in 1% increments.
     * If this feature is not supported by the interface, a call to this method return -1.
     *
     * @return The link quality in integer percent or -1 if not supported
     **/
    virtual int getLinkQuality() = 0;

    /**
     *  Get the long term (complete) mean of the data rate
     *
     * The mean of the total data rate. It is calculated as
     * all transferred bits / total link uptime.
     *
     * @return The mean data rate of the interface in bit per second, 0 if unknown
     * @see getNominalDataRate() For the nominal data rate of the interface
     * @see getShortTermDataRate() For a the mean data rate of the last seconds
     * @see getCurrentDataRate() For the data rate of the last transferred chunk
     * @see getMaxDataRate() For the maximum data rate
     **/
    virtual qint64 getTotalUpstream() = 0;

    /**
     *  Get the current data rate
     *
     * The datarate of the last 100 ms
     *
     * @return The mean data rate of the interface in bit per second, 0 if unknown
     * @see getNominalDataRate() For the nominal data rate of the interface
     * @see getLongTermDataRate() For the mean data rate
     * @see getShortTermDataRate() For a the mean data rate of the last seconds
     * @see getMaxDataRate() For the maximum data rate
     **/
    virtual qint64 getCurrentUpstream() = 0;

    /**
     *  Get the maximum data rate
     *
     * The maximum peak data rate.
     *
     * @return The mean data rate of the interface in bit per second, 0 if unknown
     * @see getNominalDataRate() For the nominal data rate of the interface
     * @see getLongTermDataRate() For the mean data rate
     * @see getShortTermDataRate() For a the mean data rate of the last seconds
     * @see getCurrentDataRate() For the data rate of the last transferred chunk
     **/
    virtual qint64 getMaxUpstream() = 0;

    /**
     *  Get the total number of bits sent
     *
     * @return The number of sent bits
     **/
    virtual qint64 getBitsSent() = 0;

    /**
     *  Get the total number of bits received
     *
     * @return The number of received bits
     * @bug Decide if the bits should be counted fromt the instantiation of the interface or if the counter should reset on disconnect.
     **/
    virtual qint64 getBitsReceived() = 0;

    /**
     *  Connect this interface logically
     *
     * @return True if connection could be established, false otherwise
     **/
    virtual bool connect() = 0;

    /**
     *  Disconnect this interface logically
     *
     * @return True if connection could be terminated, false otherwise
     **/
    virtual bool disconnect() = 0;

    /**
     *  Get the current number of bytes in buffer.
     *
     * @return The number of bytes ready to read
     **/
    virtual qint64 bytesAvailable() = 0;

    /**
     *  Informs the link manager that a loss of link is expected (don't emit error)
     **/
    virtual void linkLossExpected(const bool yes) { Q_UNUSED(yes) }

public slots:

    /**
     *  This method allows to write bytes to the interface.
     *
     * If the underlying communication is packet oriented,
     * one write command equals a datagram. In case of serial
     * communication arbitrary byte lengths can be written
     *
     * @param bytes The pointer to the byte array containing the data
     * @param length The length of the data array
     **/
    virtual void writeBytes(const char *bytes, qint64 length) = 0;

signals:

    /**
     *  New data arrived
     *
     * The new data is contained in the QByteArray data. The data is copied for each
     * receiving protocol. For high-speed links like image transmission this might
     * affect performance, for control links it is however desirable to directly
     * forward the link data.
     *
     * @param data the new bytes
     */
    void bytesReceived(LinkInterface* link, QByteArray data);

    void teleReceived(QByteArray data, int rows, int cols);

    /**
     *  This signal is emitted instantly when the link is connected
     **/
    void connected();

    /**
     *  This signal is emitted instantly when the link is disconnected
     **/
    void disconnected();

    /**
     *  This signal is emitted instantly when the link status changes
     **/
    void connected(bool connected);

    /**
     *  This signal is emitted if the human readable name of this link changes
     */
    void nameChanged(QString name);

    /**  Communication error occured */
    void communicationError(const QString& linkname, const QString& error);

    /**  destroying element */
    void deleteLink(LinkInterface* const link);

protected:
    static int getNextLinkId() {
        static int nextId = 1;
        return nextId++;
    }

protected slots:

    /**
     *  Read a number of bytes from the interface.
     *
     * @param bytes The pointer to write the bytes to
     * @param maxLength The maximum length which can be written
     **/
    virtual void readBytes() = 0;

};

#endif // _LINKINTERFACE_H_
