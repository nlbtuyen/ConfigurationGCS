import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: root

    property alias x: translation.dx
    property alias y: translation.dy
    property alias z: translation.dz
    property alias scale: scaleTransform.scale
//    property alias theta: thetaRotation.angle
//    property alias phi: phiRotation.angle

    property real altitude : 5
    property Material material

    components: [ transform, mesh, root.material ]

    Transform {
        id: transform
        property real rollAngle : 0
        property real pitchAngle : 15
        Translate { id: translation }
        Scale { id: scaleTransform }
//        Rotate{ id: thetaRotation; axis: Qt.vector3d( 1.0, 0.0, 0.0 ) }
//        Rotate{ id: phiRotation;   axis: Qt.vector3d( 0.0, 1.0, 0.0 ) }
        Rotate { // roll
            axis : Qt.vector3d(1, 0, 0)
            angle : transform.rollAngle
        }

        Rotate { // pitch
            axis : Qt.vector3d(0, 0, 1)
            angle : transform.pitchAngle
        }
        Rotate {angle : -90; axis : Qt.vector3d(1, 0, 0)}
        Rotate {angle : -180; axis : Qt.vector3d(0, 1, 0)}
    }
    Mesh {
        id: mesh
        source: "qrc:/3dmodel/Drone.obj"
    }
    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.ParallelAnimation {
            QQ2.SequentialAnimation {
                QQ2.NumberAnimation { target: transform; property: "pitchAngle"; from: 0; to: 30; duration: 2000; easing.type: QQ2.Easing.OutQuad }
                QQ2.NumberAnimation { target: transform; property: "pitchAngle"; from: 30; to: 0; duration: 2000; easing.type: QQ2.Easing.OutSine }
            }
//            QQ2.NumberAnimation { target: transform; property: "altitude"; to: 5; duration: 4000; easing.type: QQ2.Easing.InOutCubic }
        }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.ParallelAnimation {
            QQ2.SequentialAnimation {
                QQ2.NumberAnimation { target: transform; property: "pitchAngle"; from: 0; to: -30; duration: 1000; easing.type: QQ2.Easing.OutQuad }
                QQ2.NumberAnimation { target: transform; property: "pitchAngle"; from: -30; to: 0; duration: 5000; easing.type: QQ2.Easing.OutSine }
            }
//            QQ2.NumberAnimation { target: transform; property: "altitude"; to: 0; duration: 6000; easing.type: QQ2.Easing.InOutCubic}
        }
        QQ2.PauseAnimation { duration: 1500 }
    }
    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.NumberAnimation { target: transform; property: "rollAngle"; to: 360; duration: 1500; easing.type: QQ2.Easing.InOutQuad }
        QQ2.PauseAnimation { duration: 1000 }
        QQ2.NumberAnimation { target: transform; property: "rollAngle"; from: 0; to: 30; duration: 1000; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.NumberAnimation { target: transform; property: "rollAngle"; from: 30; to: -30; duration: 1000; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.NumberAnimation { target: transform; property: "rollAngle"; from: -30; to: 0; duration: 750; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 2000 }
    }
}
