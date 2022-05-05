import QtQuick 2.0
import QtQuick.Dialogs 1.2

SidePanel{
    id: root
    title: "Цвет"
    property color currentColor: "black"

    Column{
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: resizeRect.left
        spacing: 5
        Rectangle{
            width: parent.width
            border.width: 1
            height: 36
            color: root.currentColor
            anchors.margins: 3
        }
        CustomButton{
            width: parent.width
            border.width: 1
            text: "Выбрать цвет"
            onClicked: cd.open()
        }
    }
    ColorDialog{
        id: cd
        onAccepted: root.currentColor = color
    }
}
