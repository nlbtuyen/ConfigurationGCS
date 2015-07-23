import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2
import QtQuick.Layouts 1.0

Entity {
    id: root

    signal signalQML()
    //signal receiveMess(string msg)
    function setTextField(){


    }

    // Render from the mainCamera
    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        }
    ]


    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0,0.0, 80.0)

    }

    Configuration  {
        controlledCamera: mainCamera
    }


    Model {
        id: aModel
        onXChanged: signalQML()
    }


}
