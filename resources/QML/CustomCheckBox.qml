import QtQuick 2.10
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

CheckBox {
    id: control
    text: qsTr("CheckBox")
    checked: false
    leftPadding: 0
    opacity: control.down ? 0.8 : 1.0
    font.pointSize: 10
    indicator: Rectangle {
        implicitWidth: 24
        implicitHeight: 24
        x: control.leftPadding
        y: parent.height / 2 - height / 2
//        radius: 3
        border.color: "white"
        color: Properties.backgroundColor

        Rectangle {
            width: 16
            height: width
            anchors.centerIn: parent
            color: Properties.customCheckBoxCheckedItemColor
            visible: control.checked
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: enabled ? "white" : "darkgray"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
