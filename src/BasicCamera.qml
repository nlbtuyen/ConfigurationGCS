import Qt3D 2.0
import Qt3D.Renderer 2.0


Camera {
    id: mainCamera
    projectionType: CameraLens.PerspectiveProjection
    fieldOfView: 22.5
    aspectRatio: 1
    nearPlane:   0.01
    farPlane:    1000.0
    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
}

