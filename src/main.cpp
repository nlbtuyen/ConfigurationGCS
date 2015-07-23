#include "mainwindow.h"
#include <QApplication>

#include <window.h>
#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QtQml>
#include <QQuickView>
#include "drone.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Window view;

    QWidget *container = QWidget::createWindowContainer(&view);
    container->setWindowTitle(QStringLiteral("Basic shapes"));

    Qt3D::Quick::QQmlAspectEngine engine;
    engine.aspectEngine()->registerAspect(new Qt3D::QRenderAspect());
    engine.aspectEngine()->registerAspect(new Qt3D::QInputAspect());
    // Expose the window as a context property so we can set the aspect ratio
    engine.qmlEngine()->rootContext()->setContextProperty("window",QVariant::fromValue(&view));


    Drone drone;
    drone.setRootObject(engine.qmlEngine()->rootContext());

    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.aspectEngine()->setData(data);
    engine.aspectEngine()->initialize();
    engine.setSource(QUrl("qrc:/src/main.qml"));
    w.ui->scrollArea_3D->setWidget(container);


    QObject *top = engine.qmlEngine()->rootContext();
    container = qobject_cast<QWidget *>(top);

    // connect our QML signal to our C++ slot
    QObject::connect(container, SIGNAL(signalQML()), &drone, SLOT(cppSlot()));

    // connect our C++ signal to our QML slot
    QObject::connect(&drone,SIGNAL(cppSignal()),container, SLOT(setTextField()));


//    QObject::connect(window, SIGNAL(receiveMess(QString)), &drone, SLOT(cppSlot(QString)));
//    QObject(&view, SIGNAL(receiveMess(int)), &drone, SLOT(onDisplayChanged())


    w.show();
    return a.exec();
}
