import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: model
    property Material material

    property real rollCpp: drone.roll
    property real pitchCpp: drone.pitch
    components: [ transform, mesh, model.material ]

    Transform {

        id: transform
        objectName: "MyModel"

        Rotate { // roll
            axis : Qt.vector3d(0,1, 0)
            angle : drone.roll
        }
        Rotate {
            axis: Qt.vector3d(1,0,0)
            angle: -90
        }
        Rotate {
            axis: Qt.vector3d(1,0,0)
            angle: drone.pitch
        }
    }

    Mesh {
        id: mesh
        source: "qrc:/3dmodel/Drone.obj"
    }

    //    QQ2.SequentialAnimation {
    //        QQ2.SequentialAnimation {
    //            QQ2.NumberAnimation { target: transform; property: "rollAngle"; from: 0; to: transform.rollAngle; easing.type: QQ2.Easing.OutQuad }
    //        }
    //    }

}
