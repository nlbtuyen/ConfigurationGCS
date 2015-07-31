import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: sceneRoot

    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        }
    ]
    Configuration {
        controlledCamera:  mainCamera
    }

    BasicCamera {
            id: mainCamera
            position: Qt.vector3d( 0.0, 0.0, 80.0 )
        }

    Model {
        id: model
    }

}