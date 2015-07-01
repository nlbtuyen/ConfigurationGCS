#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include <QObject>
#include <QWidget>
#include <QAction>
#include <QSettings>

#include "linkinterface.h"
#include "protocolinterface.h"
#include "ui_connectiontab.h"

class ConnectionTab : public QWidget
{
    Q_OBJECT

public:
    ConnectionTab(LinkInterface* link, ProtocolInterface* protocol, QWidget *parent = 0);
    ~ConnectionTab();
    QAction* getAction();

protected:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    void closeEvent(QCloseEvent* event);

public slots:
    void setLinkType(int linktype);
    /** @brief Set the protocol for this link */
    void setProtocol(int protocol);
    void setConnection();
    void connectionState(bool connect);
    void setLinkName(QString name);
    /** @brief Disconnects the associated link, removes it from all menus and closes the window. */
    void remove();

private slots:
    void saveSettings();
    void loadSettings();

private:
    Ui::ConnectionTab ui;
    LinkInterface* link;
    QAction* action;
    QSettings settings;
};

#endif // CONNECTIONTAB_H
