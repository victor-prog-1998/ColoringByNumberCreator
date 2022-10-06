import QtQuick 2.10
import QtQuick.Controls 2.3

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
        border.color: "black"

        Rectangle {
            width: 18
            height: width
            anchors.centerIn: parent
//            radius: 2
            color: "#137ea8"
            visible: control.checked
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: enabled ? "black" : "gray"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
