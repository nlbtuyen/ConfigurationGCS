#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include "uasinterface.h"

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget* parent = 0);
    void addPerspectiveChangeAction(QAction* action);
    void setPerspectiveChangeActions(const QList<QAction *> &actions);
    ~ToolBar();

public slots:
    /** @brief Set the system that is currently displayed by this widget */
    void setActiveUAS(UASInterface* active);
    /** @brief Set the link which is currently handled with connecting / disconnecting */
    void addLink(LinkInterface* link);
    /** @brief Remove link which is currently handled */
    void removeLink(LinkInterface* link);
    /** @brief Set the system state */
    void updateState(UASInterface* system, QString name, QString description);
    /** @brief Set the system mode */
    void updateMode(int system, QString name, QString description);
    /** @brief Update the system name */
    void updateName(const QString& name);
    /** @brief Set the MAV system type */
    void setSystemType(UASInterface* uas, unsigned int systemType);
    /** @brief Received system text message */
    void receiveTextMessage(int uasid, int componentid, int severity, QString text);
    /** @brief Update battery charge state */
    void updateBatteryRemaining(UASInterface* uas, double voltage, double percent, int seconds);
    /** @brief Update current waypoint */
    void updateCurrentWaypoint(quint16 id);
    /** @brief Update distance to current waypoint */
    void updateWaypointDistance(double distance);
    /** @brief Update arming state */
    void updateArmingState(bool armed);
    /** @brief Repaint widgets */
    void updateView();
    /** @brief Update connection timeout time */
    void heartbeatTimeout(bool timeout, unsigned int ms);

protected:
    void createCustomWidgets();
    void createUI();

    QAction* toggleLoggingAction;
    QAction* logReplayAction;
    UASInterface* mav;
    QHBoxLayout* hlayout;
    QLabel* symbolLabel;
    QLabel* toolBarNameLabel;
    QLabel* toolBarTimeoutLabel;
    QLabel* toolBarSafetyLabel;
    QLabel* toolBarModeLabel;
    QLabel* toolBarStateLabel;
    QLabel* toolBarMessageLabel;
    QProgressBar* toolBarBatteryBar;
    QLabel* toolBarBatteryVoltageLabel;
    bool changed;
    float batteryPercent;
    float batteryVoltage;
    int wpId;
    double wpDistance;
    QString state;
    QString mode;
    QString systemName;
    QString lastSystemMessage;
    QTimer updateViewTimer;
    bool systemArmed;
    QButtonGroup *group;
    LinkInterface* currentLink;

};

#endif // TOOLBAR_H
