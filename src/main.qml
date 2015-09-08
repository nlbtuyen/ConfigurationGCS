import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Scene3D 2.0

Item {
    Rectangle {
        id: screne
        width: 220;
        height: 170;
        anchors.centerIn: parent
    }
    Scene3D {
        id: dd
        anchors.fill: parent
        anchors.margins: 10
        focus: true
        aspects: "input"
        ModelView { id: modelView}
    }
}
