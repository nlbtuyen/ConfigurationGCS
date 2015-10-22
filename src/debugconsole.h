#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <QWidget>
#include <QList>
#include <QByteArray>
#include <QTimer>
#include <QKeyEvent>

#include "linkinterface.h"

namespace Ui
{
class DebugConsole;
}

/**
 *  Shows a debug console
 *
 * This class shows the raw data stream of each link
 * and the debug / text messages sent by all systems
 */
class DebugConsole : public QWidget
{
    Q_OBJECT
public:
    DebugConsole(QWidget *parent = 0);
    ~DebugConsole();

public slots:
    /**  Docking location changed */
    void dockEvent(Qt::DockWidgetArea area);
    /**  Add a link to the list of monitored links */
    void addLink(LinkInterface* link);
    /**  Remove a link from the list */
    void removeLink(LinkInterface* const link);
    /**  Update a link name */
    void updateLinkName(QString name);
    /**  A link was connected or disconnected */
    void linkStatusChanged(bool connected);
    /**  Select a link for the active view */
    void linkSelected(int linkIdx);
    /**  Set connection state of a link */
    void setLinkState(LinkInterface* link);
    /**  Set connection state of the current link */
    void setConnectionState(bool connected);
    /**  Receive bytes from link */
    void receiveBytes(LinkInterface* link, QByteArray bytes);
    /**  Send lineedit content over link */
    void sendBytes();
    /**  Enable HEX display mode */
    void hexModeEnabled(bool mode);
    /**  Filter out MAVLINK raw data */
    void MAVLINKfilterEnabled(bool filter);
    /**  Freeze input, do not store new incoming data */
    void hold(bool hold);
    /**  Handle the connect button */
    void handleConnectButton();
    /**  Enable auto-freeze mode if traffic intensity is too high to display */
    void setAutoHold(bool hold);
    /**  Receive plain text message to output to the user */
    void receiveTextMessage(int id, int component, int severity, QString text);
    /**  Append a special symbol */
    void appendSpecialSymbol(const QString& text);
    /**  Append the special symbol currently selected in combo box */
    void appendSpecialSymbol();
    /**  A new special symbol is selected */
    void specialSymbolSelected(const QString& text);
    /**  Change view type from compact to full */
    void on_checkBox_simpleView_toggled(bool checked);

protected slots:
    /**  Draw information overlay */
    void paintEvent(QPaintEvent *event);
    /**  Update traffic measurements */
    void updateTrafficMeasurements();
    void loadSettings();
    void storeSettings();

protected:
    void changeEvent(QEvent *e);
    void hideEvent(QHideEvent* event);
    /**  Convert a symbol name to the byte representation */
    QByteArray symbolNameToBytes(const QString& symbol);
    /**  Convert a symbol byte to the name */
    QString bytesToSymbolNames(const QByteArray& b);
    /**  Handle keypress events */
    void keyPressEvent(QKeyEvent * event);
    /**  Cycle through the command history */
    void cycleCommandHistory(bool up);

    QList<LinkInterface*> links;
    LinkInterface* currLink;

    bool holdOn;              ///< Hold current view, ignore new data
    bool convertToAscii;      ///< Convert data to ASCII
    bool filterMAVLINK;       ///< Set true to filter out MAVLink in output
    bool autoHold;            ///< Auto-hold mode sets view into hold if the data rate is too high
    int bytesToIgnore;        ///< Number of bytes to ignore
    char lastByte;            ///< The last received byte
    bool escReceived;         ///< True if received ESC char in ASCII mode
    int escIndex;             ///< Index of bytes since ESC was received
    char escBytes[5];         ///< Escape-following bytes
    bool terminalReceived;    ///< Terminal sequence received
    QList<QString> sentBytes; ///< Transmitted bytes, per transmission
    QByteArray holdBuffer;    ///< Buffer where bytes are stored during hold-enable
    QString lineBuffer;       ///< Buffere where bytes are stored before writing them out
    quint64 lastLineBuffer;   ///< Last line buffer emission time
    QTimer lineBufferTimer;   ///< Line buffer timer
    QTimer snapShotTimer;     ///< Timer for measuring traffic snapshots
    int snapShotInterval;     ///< Snapshot interval for traffic measurements
    int snapShotBytes;        ///< Number of bytes received in current snapshot
    float dataRate;           ///< Current data rate
    float lowpassDataRate;    ///< Lowpass filtered data rate
    float dataRateThreshold;  ///< Threshold where to enable auto-hold
    QStringList commandHistory;
    QString currCommand;
    int commandIndex;

private:
    Ui::DebugConsole *m_ui;
};

#endif // DEBUGCONSOLE_H
