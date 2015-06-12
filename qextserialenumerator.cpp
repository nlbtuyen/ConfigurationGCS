#include "qextserialenumerator.h"
#include "qextserialenumerator_p.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaType>
#include <QtCore/QRegExp>

QextSerialEnumeratorPrivate::QextSerialEnumeratorPrivate(QextSerialEnumerator *enumrator)
    :q_ptr(enumrator)
{
    init_sys();
}

QextSerialEnumeratorPrivate::~QextSerialEnumeratorPrivate()
{
    destroy_sys();
}

QextSerialEnumerator::QextSerialEnumerator(QObject *parent)
    :QObject(parent), d_ptr(new QextSerialEnumeratorPrivate(this))
{
    if (!QMetaType::isRegistered(QMetaType::type("QextPortInfo")))
        qRegisterMetaType<QextPortInfo>("QextPortInfo");
}

/*!
   Destructs the QextSerialEnumerator object.
*/
QextSerialEnumerator::~QextSerialEnumerator()
{
    delete d_ptr;
}

/*!
    Get list of ports.

    return list of ports currently available in the system.
*/
QList<QextPortInfo> QextSerialEnumerator::getPorts()
{
    return QextSerialEnumeratorPrivate::getPorts_sys();
}

/*!
    Enable event-driven notifications of board discovery/removal.
*/
void QextSerialEnumerator::setUpNotifications()
{
    Q_D(QextSerialEnumerator);
    if (!d->setUpNotifications_sys(true))
        QESP_WARNING("Setup Notification Failed...");
}
#include "moc_qextserialenumerator.cpp"
