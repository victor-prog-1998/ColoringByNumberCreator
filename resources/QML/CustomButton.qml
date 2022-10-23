import QtQuick 2.0
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    property color buttonColor: Properties.buttonColor
    property color hoverColor: Properties.buttonHoverColor
    property alias text: txt.text
    property alias textColor: txt.color
    signal clicked()
    height: 24
    width: 100
    color:buttonColor
    clip: true
    opacity: enabled ? 1.0 : 0.5
    border.width: 1
    border.color: "white"

    Text{
        id: txt
        font.pointSize: 10
        anchors.centerIn: parent
        color: "white"
    }

    MouseArea{
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.color = root.hoverColor
        onExited: root.color = root.buttonColor
        onClicked: root.clicked();
    }
}
