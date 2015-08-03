#ifndef DRONE_H
#define DRONE_H

#include "uasmanager.h"
#include <QObject>
#include <QDebug>
#include <qqml.h>
class Drone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float roll READ rollCpp NOTIFY rollChanged)
    Q_PROPERTY(float pitch READ pitchCpp NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yawCpp NOTIFY yawChanged)

public:
    Drone();
    ~Drone();
    float roll;
    float pitch;
    float heading;

    float rollCpp() const {
        return this->roll;
    }

    float pitchCpp() const {
        return this->pitch;
    }

    float yawCpp() const {
        return this->heading;
    }

signals:
    void rollChanged();
    void pitchChanged();
    void yawChanged();

public slots:
    virtual void setActiveUAS(UASInterface* uas);
    /** @brief Attitude from main autopilot / system state */
    void updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp);
    /** @brief Attitude from one specific component / redundant autopilot */
    void updateAttitude(UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp);

private:
    UASInterface* uas;          ///< The uas currently monitored
    QObject *mRoot;
    QString mDisplay;

};

#endif // DRONE_H
