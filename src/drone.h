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
public:
    Drone();
    ~Drone();
    float roll;
    float pitch;

    float rollCpp() const {
        return this->roll;
    }

    float pitchCpp() const {
        return this->pitch;
    }


signals:
    void rollChanged();
    void pitchChanged();

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
    float heading;

};

#endif // DRONE_H
