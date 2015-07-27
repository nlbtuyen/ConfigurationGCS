#include "drone.h"
#include "uasmanager.h"
#include <QClipboard>
#include <QDebug>
#include <stdexcept>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>

#ifndef isinf
#define isinf(x) ((x)!=(x))
#endif


#ifndef isnan
#define isnan(x) ((x)!=(x))
#endif

Drone::Drone(QObject *parent):
    QObject(parent),
    mRoot(0),
    mDisplay()
{
    //connect(this, SIGNAL(displayChanged()), this, SLOT(onDisplayChanged()));
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

}

void Drone::setRoll(float r)
{
    if (r == this->roll)
        return;
    r = this->roll;
    emit rollChanged(r);
}

void Drone::setRootObject(QObject *root)
{
    // disconnect from any previous root
    if (mRoot != 0) mRoot->disconnect(this);

    mRoot = root;


//    return mRoot;

}

void Drone::setDisplay(const QString &display)
{
    if (mDisplay != display) {
        mDisplay = display;
//        emit displayChanged();
    }
}

void Drone::setActiveUAS(UASInterface *uas)
{

    if (uas)
    {
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*, double, double, double, quint64)));
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,int,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*,int,double, double, double, quint64)));
        this->uas= uas;
    }
}

void Drone::updateAttitude(UASInterface *uas, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw))
    {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
    setRoll(this->roll);
//    qDebug("r,p,y: %f,%f,%f", roll, pitch, yaw);

    QQmlEngine engine;
    QQmlComponent component(&engine, QUrl("qrc:/src/Model.qml"));
    QObject *object = component.create();

    QObject *childObject = object->findChild<QObject *>("MyModel");

    childObject->setProperty("rollAngle",this->roll);
    qDebug() << "Property value:" << QQmlProperty::read(childObject, "rollAngle").toFloat();
    engine.destroyed();

}

void Drone::updateAttitude(UASInterface *uas, int component, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw)) {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

//void Drone::cppSlot()
//{
//    qDebug() << "c++: HandleTextField::handleSubmitTextField:";
//    emit cppSignal();

//}

