#ifndef COMMCONFIGURATIONWINDOW_H
#define COMMCONFIGURATIONWINDOW_H

#include <QObject>
#include <QWidget>
#include <QAction>
#include <QSettings>
#include "linkinterface.h"
#include "protocolinterface.h"
#include "ui_commsettings.h"


enum qgc_link_t {
    QGC_LINK_SERIAL,
    QGC_LINK_UDP,
    QGC_LINK_SIMULATION,
    QGC_LINK_FORWARDING
};

enum qgc_protocol_t {
    QGC_PROTOCOL_MAVLINK,
};


class CommConfigurationWindow : public QWidget
{
    Q_OBJECT

public:
    CommConfigurationWindow(LinkInterface* link, ProtocolInterface* protocol, QWidget *parent = 0);
    ~CommConfigurationWindow();

    QAction* getAction();

protected:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    void closeEvent(QCloseEvent* event);

public slots:
    void setLinkType(int linktype);
    // Set the protocol for this link
    void setProtocol(int protocol);
    void setConnection();
    void connectionState(bool connect);
    void setLinkName(QString name);
    // Disconnects the associated link, removes it from all menus and closes the window.
    void remove();

private slots:
    void saveSettings();
    void loadSettings();

private:

    Ui::commSettings ui;
    LinkInterface* link;
    QAction* action;
    QSettings settings;
};

#endif // COMMCONFIGURATIONWINDOW_H
