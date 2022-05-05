import QtQuick 2.0
import QtQuick.Controls 2.5

Rectangle{
    id: root
    height: 50
    property bool changeColorMode: false
    property bool pencilMode: false
    property bool fillMode: false

    signal openButtonClicked()
    signal edgesButtonClicked()
    signal paletteButtonClicked()


    property alias modeComboBox: modeComboBox
    property alias paletteButton: paletteButton

    color: "#3ddb72"
    Row{
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        spacing: 5
        ImageButton{
            source: "qrc:/icons/folder-icon.png"
            toolTipText: "Открыть изображение"
            onClicked: root.openButtonClicked()
        }
        ImageButton{
            id: paletteButton
            source: "qrc:/icons/palette-icon.png"
            toolTipText: "Палитра и постеризация"
            onClicked: root.paletteButtonClicked()
            visible: false
        }



        ComboBox{
            id: modeComboBox
            model: ["Оригинал", "Постеризация","Края"]
            height: 48
        }

        ImageButton{
            source: root.changeColorMode ? "qrc:/icons/cancel-change-color-icon.png" : "qrc:/icons/change-color-icon.png"
            onClicked: root.changeColorMode = !root.changeColorMode
            toolTipText: root.changeColorMode ? "Отмена" : "Замена цвета"
            visible: modeComboBox.currentIndex == 1
        }

        ImageButton{
            source: "qrc:/icons/fill-icon.png"
            toolTipText: "Залить"
            visible: modeComboBox.currentIndex == 1
            onClicked: root.fillMode = true;
        }

        ImageButton{
            source: "qrc:/icons/pencil-icon.png"
            toolTipText: "Карандаш"
            visible: modeComboBox.currentIndex == 1
            onClicked: root.pencilMode = true
        }



    }
}
