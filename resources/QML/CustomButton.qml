import QtQuick 2.0

Rectangle{
    id: root
    property color buttonColor: "white"
    property color hoverColor: "lightgray"
    property alias text: txt.text
    property alias textColor: txt.color
    signal clicked()
    height: 36
    width: 100
    color:buttonColor
    clip: true
    opacity: enabled ? 1.0 : 0.5

    Text{
        id: txt
        font.pointSize: 14
        anchors.centerIn: parent
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
