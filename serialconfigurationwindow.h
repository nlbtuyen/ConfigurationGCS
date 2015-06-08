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
#include "seriallinkinterface.h"
#include "ui_serialsettings.h"

class SerialConfigurationWindow : public QWidget
{
    Q_OBJECT

public:
    SerialConfigurationWindow(LinkInterface* link, QWidget *parent = 0, Qt::WindowFlags flags = Qt::Sheet);
    ~SerialConfigurationWindow();

    QAction* getAction();

public slots:
    void configureCommunication();
    void setupPortList();
    void setFlowControlNone(bool accept);
    void setFlowControlHw(bool accept);
    void setFlowControlSw(bool accept);
    void setParityNone(bool accept);
    void setParityOdd(bool accept);
    void setParityEven(bool accept);
    void setDataBits(QString bits);
    void setStopBits(QString bits);
    void setPortName(QString port);
    void setLinkName(QString name);

protected:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    bool userConfigured; ///< Switch to detect if current values are user-selected and shouldn't be overriden

protected slots:
    void portError(const QString &err);

private:

    Ui::serialSettings ui;
    SerialLinkInterface* link;
    QAction* action;
    QTimer* portCheckTimer;
    bool mtx_portListUpdating;

};
#endif // SERIALCONFIGURATIONWINDOW_H
