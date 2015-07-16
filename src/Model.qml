import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity {
    id: root

    property Material material

    components: [ transform, mesh, root.material ]

    Transform {
        id: transform
    }

    Mesh {
        id: mesh
        source: "rocket.obj"
    }
}
