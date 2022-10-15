import QtQuick 2.0
import QtQuick.Controls 2.12

Popup{
    id: root
    width: 200
    height: 70
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 5
    signal accepted(var value)
    contentItem: Item {
        Text{
            anchors.top: parent.top
            anchors.left: parent.left
            text: "Число цветов"
            font.pointSize: 12
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
            height: 30
            border.width: 1
            onClicked: {
                root.accepted(spinBox.value)
                root.close()
            }
        }

    }
}
