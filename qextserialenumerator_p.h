#ifndef QEXTSERIALENUMERATOR_P_H
#define QEXTSERIALENUMERATOR_P_H


#include "qextserialenumerator.h"


#  include <QtCore/qt_windows.h>


class QextSerialRegistrationWidget;
class QextSerialEnumeratorPrivate
{
    Q_DECLARE_PUBLIC(QextSerialEnumerator)
public:
    QextSerialEnumeratorPrivate(QextSerialEnumerator *enumrator);
    ~QextSerialEnumeratorPrivate();
    void init_sys();
    void destroy_sys();

    static QList<QextPortInfo> getPorts_sys();
    bool setUpNotifications_sys(bool setup);

    LRESULT onDeviceChanged(WPARAM wParam, LPARAM lParam);
    bool matchAndDispatchChangedDevice(const QString &deviceID, const GUID &guid, WPARAM wParam);
    QextSerialRegistrationWidget *notificationWidget;

private:
    QextSerialEnumerator *q_ptr;
};
#endif // QEXTSERIALENUMERATOR_P_H
