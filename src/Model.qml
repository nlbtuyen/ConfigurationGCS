import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: root

    property alias x: translation.dx
    property alias y: translation.dy
    property alias z: translation.dz
    property alias scale: scaleTransform.scale
    property Material material

    components: [ transform, mesh, root.material ]

    Transform {
        id: transform
        property real rollAngle : 0
        property real pitchAngle : 20
        Translate { id: translation }
        Scale { id: scaleTransform }

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

}
