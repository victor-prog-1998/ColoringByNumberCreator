import QtQuick 2.0
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

Popup{
    id: root
    height: 70
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 1
    signal accepted(var value)
    contentItem: Rectangle {
        color: Properties.backgroundColor
        border.width: 1
        border.color: "white"
        Item {
            anchors.fill: parent
            anchors.margins: 4
            Text{
                anchors.top: parent.top
                anchors.left: parent.left
                text: "Число цветов"
                color: "white"
                font.pointSize: 10
            }

            CustomSpinBox{
                id: spinBox
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                value: 2
                minimum: 2
                maximum: 256
            }

            CustomButton{
                text: "Ок"
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: spinBox.right
                anchors.leftMargin: 5
                border.width: 1
                onClicked: {
                    root.accepted(spinBox.value)
                    root.close()
                }
            }
        }


    }
}
