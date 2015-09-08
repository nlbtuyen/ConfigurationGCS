#ifndef SERIALCONFIGURATIONWINDOW_H
#define SERIALCONFIGURATIONWINDOW_H


#include <QObject>
#include <QWidget>
#include <QAction>
#include <QTimer>
#include <QShowEvent>
#include <QHideEvent>
#include <QMutex>

#include "linkinterface.h"
#include "ui_serialsettings.h"

class QextSerialEnumerator;
class SerialLinkInterface;

class SerialConfigurationWindow : public QWidget
{
    Q_OBJECT

public:
    SerialConfigurationWindow(LinkInterface* link, QWidget *parent = 0, Qt::WindowFlags flags = Qt::Sheet);
    ~SerialConfigurationWindow();

    QAction* getAction();

public slots:
    void loadSettings();
    void writeSettings();
    QString getSettingsKey(bool checkExists);
    void loadPortSettings();
    void writePortSettings();
    void configureCommunication();
    void setupPortList();
    void setFlowControl(int fc);
    void setParity(int parity);
    void setBaudRate(QString rate);
    void setDataBits(QString bits);
    void setStopBits(QString bits);
    void setPortName(QString port);
    void setLinkName(QString name);
    void setTimeoutMs(int to);
    void setReconnectDelay(int dly);

protected:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    bool userConfigured; ///< Switch to detect if current values are user-selected and shouldn't be overriden

protected slots:
    void portError(const QString &err);


private:

    Ui::serialSettings ui;
    SerialLinkInterface* link;
    QextSerialEnumerator *portEnumerator;
    QAction* action;
    QString defaultPortName;

};

#endif // SERIALCONFIGURATIONWINDOW_H
