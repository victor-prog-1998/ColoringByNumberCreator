import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle{
    id: root
    width: icon.width + padding * 2
    height: icon.height + padding * 2
    property int padding: 0
    property alias icon: icon
    property color buttonColor: "transparent"
    property color hoverColor: "transparent"
    property alias source: icon.source
    scale: ma.pressed ? 0.9 : ma.containsMouse ? 1.1 : 1.0
    opacity: enabled ? 1.0 : 0.3
    property string toolTipText: ""

    color: buttonColor

    signal clicked()
    Image {
        id: icon
        anchors.centerIn: parent
        MouseArea{
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onEntered: root.color = root.hoverColor
            onExited: root.color = root.buttonColor
            onClicked: root.clicked();
            ToolTip.text: root.toolTipText
            ToolTip.visible: root.toolTipText == "" ? false : containsMouse
        }
    }
}
