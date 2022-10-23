import QtQuick 2.0
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    height: 28
    color: Properties.headerColor
    property bool changeColorMode: false
    property bool pencilMode: false
    property bool fillMode: false
    property int modeComboBoxOriginalIndex: 0
    property int modeComboBoxPosterizedlIndex: 1
    property int modeComboBoxColoringIndex: 2
    property int coloringComboBoxColoringIndex: 0
    property int coloringComboBoxPaintedIndex: 1
    property int coloringComboBoxLegendIndex: 2
    signal openButtonClicked()
    signal saveButtonClicked()
    signal edgesButtonClicked()
    signal paletteButtonClicked()
    signal settingsButtonClicked()
    signal changeColorButtonClicked()

    property var modeComboBoxShortModel: ["Оригинал", "Постеризация"]
    property var modeComboBoxFullModel: ["Оригинал", "Постеризация", "Раскраска"]

    property alias modeComboBox: modeComboBox
    property alias paletteButton: paletteButton
    property alias coloringComboBox: coloringComboBox
    property alias createColoringButton: createColoringButton

    onChangeColorModeChanged: {
        if(changeColorMode)
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = pencilButton.enabled =
                    fillButton.enabled = false;
        }
        else
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = pencilButton.enabled =
                    fillButton.enabled = true;
        }
        changeColorButtonClicked();
    }

    onFillModeChanged: {
        if(fillMode)
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = pencilButton.enabled =
                    changeColorButton.enabled = false;
        }
        else
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = pencilButton.enabled =
                    changeColorButton.enabled = true;
        }
    }

    onPencilModeChanged: {
        if(pencilMode)
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = fillButton.enabled =
                    changeColorButton.enabled = false;
        }
        else
        {
            openImageButton.enabled = paletteButton.enabled =
                    modeComboBox.enabled = fillButton.enabled =
                    changeColorButton.enabled = true;
        }
    }

    Row{
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        spacing: 5
        ImageButton{
            id: openImageButton
            source: "qrc:/icons/folder-icon.png"
            toolTipText: "Открыть изображение"
            onClicked: root.openButtonClicked()
        }
        ImageButton{
            id: settingsImageButton
            source: "qrc:/icons/settings-icon.png"
            toolTipText: "Настройки"
            onClicked: root.settingsButtonClicked()
        }
        ImageButton{
            id: saveButton
            source: "qrc:/icons/save-icon.png"
            toolTipText: "Сохранить"
            onClicked: root.saveButtonClicked()
            visible: modeComboBox.currentIndex == root.modeComboBoxColoringIndex
        }

        ImageButton{
            id: paletteButton
            source: "qrc:/icons/palette-icon.png"
            toolTipText: "Палитра и постеризация"
            onClicked: root.paletteButtonClicked()
            visible: false
        }

        CustomComboBox{
            id: modeComboBox
            height: 24
        }

        CustomButton{
            id: createColoringButton
            text: "Создать раскраску"
            width: 120
            border.color: Properties.createColoringButtonBorderColor
        }

        ImageButton{
            id: changeColorButton
            source: root.changeColorMode ? "qrc:/icons/cancel-change-color-icon.png" : "qrc:/icons/change-color-icon.png"
            onClicked: root.changeColorMode = !root.changeColorMode
            toolTipText: root.changeColorMode ? "Отмена" : "Замена цвета"
            visible: modeComboBox.currentIndex == root.modeComboBoxPosterizedlIndex
        }

        ImageButton{
            id: fillButton
            source: "qrc:/icons/fill-icon.png"
            toolTipText: "Залить"
            visible: modeComboBox.currentIndex == root.modeComboBoxPosterizedlIndex
            onClicked: root.fillMode = true;
        }

        ImageButton{
            id: pencilButton
            source: "qrc:/icons/pencil-icon.png"
            toolTipText: "Карандаш"
            visible: modeComboBox.currentIndex == root.modeComboBoxPosterizedlIndex
            onClicked: root.pencilMode = true
        }

        CustomComboBox{
            id: coloringComboBox
            visible: modeComboBox.currentIndex == root.modeComboBoxColoringIndex
            model: ["Раскраска по номерам", "Готовое изображение", "Легенда"]
            height: 24
            width: 200
        }
    }
}
