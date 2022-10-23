import QtQuick 2.0

Item {
    id: root
    property bool active: false
    property real scalePhase: 0.0
    visible: active
    width: 22 + 6 * Math.sin(scalePhase)
    height: width

    PropertyAnimation{
        target: root
        running: root.active
        properties: "scalePhase"
        from: 0.0
        to: Math.PI * 2
        duration: 2000
        loops: PropertyAnimation.Infinite
    }
    PropertyAnimation{
        target: root
        running: root.active
        properties: "rotation"
        from: 0
        to: 360
        duration: 600
        loops: PropertyAnimation.Infinite
    }

    Rectangle{
        anchors.top:parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: 8
        height: width
        radius: 4
        color: "red"
    }
    Rectangle{
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        width: 8
        height: width
        radius: 4
        color: "yellow"
    }
    Rectangle{
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: 8
        height: width
        radius: 4
        color: "lime"
    }
    Rectangle{
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 8
        height: width
        radius: 4
        color: "navy"
    }
}
