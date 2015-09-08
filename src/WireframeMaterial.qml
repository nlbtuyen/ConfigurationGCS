import Qt3D 2.0
import Qt3D.Renderer 2.0

Material {
    id: root

    property color ambient: Qt.rgba(0.1, 0.1, 0.1, 1.0)
    property color diffuse: Qt.rgba(0.7, 0.7, 0.9, 1.0)
    property color specular: Qt.rgba(0.1, 0.1, 0.1, 1.0)
    property real shininess: 150.0

    parameters: [
        Parameter { name: "ambient"; value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter { name: "diffuse"; value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
        Parameter { name: "specular"; value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess }
    ]
}
