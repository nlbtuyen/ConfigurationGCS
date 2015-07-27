#include "mainwindow.h"
#include <QApplication>

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QtQml>
#include <QtQuick/QQuickView>
#include "drone.h"
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QQuickView view;
    QWidget *container = QWidget::createWindowContainer(&view);

    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/src/main.qml"));

    w.ui->scrollArea_3D->setWidget(container);
    
    

    w.show();
    return a.exec();
}
