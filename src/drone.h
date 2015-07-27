#ifndef DRONE_H
#define DRONE_H

#include "uasmanager.h"
#include <QObject>
#include <QDebug>
class Drone : public QObject
{
    Q_OBJECT

public:
    Drone(QObject *parent = 0);

    float roll;

    void setRoll(float r);

    void setRootObject(QObject *root);
    QString display() const { return mDisplay; }

public slots:
    void setDisplay(const QString &display);

    virtual void setActiveUAS(UASInterface* uas);
    /** @brief Attitude from main autopilot / system state */
    void updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp);
    /** @brief Attitude from one specific component / redundant autopilot */
    void updateAttitude(UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp);


signals:
    void roolChanged(float roll);

private:
    UASInterface* uas;          ///< The uas currently monitored
    QObject *mRoot;
    QString mDisplay;
    float pitch;
    float heading;
};

#endif // DRONE_H
