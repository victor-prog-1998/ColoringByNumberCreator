import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import com.MyTypes.ImageProcessor 1.0

Window {
    ImageProcessor{
        id: imageProcessor
    }
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Создание раскрасок по номерам")
    property string imageOriginalSource
    property string posterizedImageSource: "image://provider/posterized"
    property string edgesImageSource: "image://provider/edges"
    property string coloringImageSource: "image://provider/coloring"
    property string paintedImageSource: "image://provider/painted"
    property string legendImageSource: "image://provider/legend"
    property alias changeColorMode: pageHeader.changeColorMode
    property int changeColorX
    property int changeColorY
    property bool posterized: false
    PageHeader{
        id: pageHeader
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onOpenButtonClicked: fileDialog.open()
        onSettingsButtonClicked: {
            loadSettings();
            settingsDialog.open()
        }
        onSaveButtonClicked: saveDialog.open()
        modeComboBox.visible: root.posterized
        modeComboBox.onActivated: { // выбрано пользователем
            switch(index)
            {
            case pageHeader.modeComboBoxOriginalIndex:
                imageArea.source = root.imageOriginalSource;
                break;
            case pageHeader.modeComboBoxPosterizedlIndex:
                imageArea.source = root.posterizedImageSource;
                break;
            case pageHeader.modeComboBoxColoringIndex:
                imageProcessor.coloring();
                coloringComboBox.currentIndex = pageHeader.coloringComboBoxColoringIndex;
                imageArea.source = root.coloringImageSource;
            }
        }

        coloringComboBox.onActivated: {
            switch(index)
            {
            case pageHeader.coloringComboBoxColoringIndex:
                imageArea.source = root.coloringImageSource;
                break;
            case pageHeader.coloringComboBoxPaintedIndex:
                imageArea.source = root.paintedImageSource;
                break;
            case pageHeader.coloringComboBoxLegendIndex:
                imageArea.source = root.legendImageSource;
                break;
            }
        }

        onChangeColorButtonClicked: {
            footerText.text = changeColorMode ?
                        "Укажите точку с цветом, который хотите заменить" : ""
        }

        onPaletteButtonClicked: {
            paletteSidePanel.open();
        }

        onPencilModeChanged: {
            if(pencilMode)
            {
                paletteSidePanel.close()
                colorSidePanel.open();
                footerText.text = "Укажите пиксель, которому хотите задать выбранный цвет"
            }
            else
                footerText.text = ""
        }
        onFillModeChanged: {
            if(fillMode)
            {
                paletteSidePanel.close()
                colorSidePanel.open();
                footerText.text = "Укажите область, которую хотите залить выбранным цветом"
            }
            else
                footerText.text = ""
        }
    }
    Item{
        anchors.top: pageHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: pageFooter.top

        Item{
            id: sidePanelItem
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: paletteSidePanel.visible ? paletteSidePanel.width : colorSidePanel.visible ? colorSidePanel.width : 0
            PaletteSidePanel{
                id: paletteSidePanel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                visible: false
                onPosterizeButtonClicked: {
                    imageProcessor.setColors(paletteSidePanel.getColors())
                    imageProcessor.posterize();
                    footerText.text = ""
                    imageArea.source = "";
                    imageArea.source = root.posterizedImageSource
                    root.posterized = true;
                    pageHeader.modeComboBox.currentIndex = pageHeader.modeComboBoxPosterizedlIndex;
                }
            }

            ColorSidePanel{
                id: colorSidePanel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                visible: false
                onClosed: pageHeader.pencilMode = pageHeader.fillMode = false;
            }
        }

        ImageArea{
            id: imageArea
            changeColorMode: root.changeColorMode

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: sidePanelItem.right
            onClicked: {
                if(root.changeColorMode)
                {
                    imageProcessor.removeEdgesFromProvider();
                    imageProcessor.removeColoringFromProvider();
                    root.changeColorX = x;
                    root.changeColorY = y;
                    changeColorDialog.open();
                }
                else if(pageHeader.pencilMode)
                {
                    imageProcessor.removeEdgesFromProvider();
                    imageProcessor.removeColoringFromProvider();
                    imageArea.source = "";
                    imageProcessor.setPixelColor(x, y, colorSidePanel.currentColor);
                    imageArea.source = "image://provider/posterized";
                }
                else if(pageHeader.fillMode)
                {
                    imageProcessor.removeEdgesFromProvider();
                    imageProcessor.removeColoringFromProvider();
                    imageArea.source = "";
                    imageProcessor.fill(x, y, colorSidePanel.currentColor);
                    imageArea.source = "image://provider/posterized";
                }
            }
        }
    }

    Rectangle{
        id: pageFooter
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        color: pageHeader.color
        Text{
            id: footerText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            font.pointSize: 11
            text: "Откройте изображение"
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            font.pointSize: 9
            text: Math.floor(imageArea.zoom * 100) + " %";
        }
    }

    FileDialog{
        id: fileDialog
        modality: Qt.WindowModal
        title: "Выберите изображение"
        onAccepted: {
            if(imageProcessor.setCurrentImage(fileUrl))
            {
                root.imageOriginalSource = imageArea.source = fileUrl
                paletteSidePanel.open();
                pageHeader.modeComboBox.currentIndex = pageHeader.modeComboBoxOriginalIndex;
                pageHeader.paletteButton.visible = true;
                root.posterized = false;
                footerText.text = "Задайте палитру и постеризуйте изображение"
            }
        }
    }

    FileDialog{
        id: saveFileDialog
        selectFolder: true
        modality: Qt.WindowModal
        title: "Укажите путь для сохранения файлов"
        onAccepted: {
            folderTextField.text = String(folder).substring(8);
        }
    }

    MessageDialog{
        id: messageDialog
        visible: false
    }

    Dialog{
        id: settingsDialog
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
                    id: settingsColorSelector
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
                onClicked: {
                    saveSettings();
                    imageProcessor.removeColoringFromProvider();
                    settingsDialog.close();
                    applySettings();
                }
            }
        }
    }

    Dialog{
        title: "Сохранение"
        id: saveDialog
        width: 500
        height: 300
        contentItem: Item {
            anchors.fill: parent
            anchors.margins: 10
            Column{
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                Text{
                    font.pointSize: 12
                    text: "Папка сохранения"
                    horizontalAlignment: Text.AlignLeft
                }

                RowLayout{
                    width: parent.width
                    TextField{
                        id: folderTextField
                        Layout.fillWidth: true
                        selectByMouse: true
                        font.pointSize: 10
                        background: Rectangle{
                            implicitHeight: 30
                            border.color: "darkgray"
                            border.width: 1
                        }
                    }
                    CustomButton{
                        text: "Открыть"
                        height: 30
                        border.width: 1
                        onClicked: saveFileDialog.open()
                    }
                }
                CustomCheckBox{
                    id: tileCheckBox
                    text: "Нарезка"
                }
                Row{
                    visible: tileCheckBox.checked
                    width: parent.width
                    spacing: 5
                    Text{
                        font.pointSize: 12
                        text: "По высоте"
                    }
                    CustomSpinBox{
                        id: rowsTileSpinBox
                        value: 2
                        minimum: columnTileSpinBox.value > 1 ? 1 : 2
                    }
                    Text{
                        font.pointSize: 12
                        text: "По длине"
                    }
                    CustomSpinBox{
                        id: columnTileSpinBox
                        value: 2
                        minimum: rowsTileSpinBox.value > 1 ? 1 : 2
                    }
                }
            }
            CustomButton{
                enabled: folderTextField.text.length > 0
                text: "Сохранить"
                height: 30
                width: 110
                border.width: 1
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                onClicked: {
                    let folder = folderTextField.text;
                    if(tileCheckBox.checked)
                        imageProcessor.saveResults(folder, rowsTileSpinBox.value, columnTileSpinBox.value);
                    else
                        imageProcessor.saveResults(folder);
                    saveDialog.close();
                }
            }
        }
    }

    ColorDialog{
        id: changeColorDialog
        title: "Укажите новый цвет"
        onAccepted: {
            imageArea.source = "";
            imageProcessor.changeColor(root.changeColorX, root.changeColorY, color);
            imageArea.source = "image://provider/posterized";
            pageHeader.changeColorMode = false;
        }
        onRejected: {
            pageHeader.changeColorMode = false
        }
    }

    function loadSettings()
    {
        let simplify = configManager.simplify();
        let coloringColor = configManager.coloringColor();
        let scalingFactor = configManager.scalingFactor();
        if(scalingFactor > 1)
        {
            scalingCheckBox.checked = true;
            scalingSpinBox.value = scalingFactor;
            for(var i = 0; i < scalingSpinBox.values.length; ++i)
                if(scalingSpinBox.values[i] === scalingFactor)
                {
                    scalingSpinBox.fixedIndex = i;
                    break;
                }
        }
        else
            scalingCheckBox.checked = false;

        settingsColorSelector.setCurrentColor(coloringColor);
        highDetailizationCheckBox.checked = !simplify;
    }

    function saveSettings()
    {
        let simplify = !highDetailizationCheckBox.checked;
        let coloringColor = settingsColorSelector.getCurrentColor();
        let scalingFactor = scalingCheckBox.checked ? scalingSpinBox.value : 1
        configManager.setConfigs(simplify, coloringColor, scalingFactor);
    }

    function applySettings()
    {
        if(pageHeader.modeComboBox.currentIndex == pageHeader.modeComboBoxColoringIndex)
        {
            imageProcessor.coloring();
            imageArea.source = "";
            imageArea.source = root.coloringImageSource;
            pageHeader.coloringComboBox.currentIndex = pageHeader.coloringComboBoxColoringIndex
        }
    }
}
