import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

Dialog{
    id: root
    property alias scalingCheckBox: scalingCheckBox
    property alias colorSelector: colorSelector
    property alias highDetailizationCheckBox: highDetailizationCheckBox
    property alias scalingSpinBox: scalingSpinBox
    signal saveButtonClicked()
    title: "Настройки"
    width: 420
    height: 300
    contentItem: Item {
        anchors.fill: parent
        anchors.margins: 10
        ColumnLayout{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 5
            Label{
                text: "Цвет контуров и цифр"
                font.pointSize: 12
            }
            ColorSelector{
                id: colorSelector
            }
            CustomCheckBox{
                text: "Высокая детализация"
                id: highDetailizationCheckBox
            }
            CustomCheckBox{
                text: "Масштабирование раскраски"
                id: scalingCheckBox
            }
            Text{
                font.pixelSize: 10
                visible: scalingCheckBox.checked
                text: "Масштабирование позоляет увеличить конутура,\n" +
                      "сделать их более гладкими и вместить больше числовых меток"
            }
            Row{
                visible: scalingCheckBox.checked
                width: parent.width
                spacing: 5
                Text{
                    font.pointSize: 12
                    text: "Коэффициент"
                }
                CustomSpinBox{
                    id: scalingSpinBox
                    fixedValues: true
                    values: [2, 3, 4, 6, 8, 9, 12]
                }
            }
        }
        CustomButton{
            text: "Сохранить"
            height: 30
            width: 110
            border.width: 1
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onClicked: root.saveButtonClicked()
        }
    }
}
