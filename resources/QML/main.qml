import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import com.MyTypes.ImageProcessor 1.0
import "../js/Properties.js" as Properties

Window {
    ImageProcessor{
        id: imageProcessor
        onFindPaletteFinished: {
            paletteSidePanel.setPalette(palette);
            paletteSidePanel.findPaletteProcess = false;
            pageFooter.busyIndicator.active = false;
        }
        onPosterizationFinished: {
            pageFooter.busyIndicator.active = false;
            paletteSidePanel.posterizationProcess = false;
            imageArea.source = "";
            imageArea.source = root.posterizedImageSource
            root.posterized = true;
            pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
            pageHeader.modeComboBox.currentIndex = pageHeader.modeComboBoxPosterizedlIndex;
        }
        onColoringFinished: {
            pageFooter.busyIndicator.active = false;
            pageHeader.modeComboBox.model = pageHeader.modeComboBoxFullModel
            pageHeader.modeComboBox.currentIndex = pageHeader.modeComboBoxColoringIndex;
            pageHeader.coloringComboBox.currentIndex = pageHeader.coloringComboBoxColoringIndex;
            imageArea.source = "";
            imageArea.source = root.coloringImageSource;
            createColoringProcess = false;
        }

        onMessage: {
            pageFooter.footerText.text = message;
        }
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
    property bool createColoringProcess: false
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
        createColoringButton.visible: root.posterized && modeComboBox.model.length < 3
        createColoringButton.enabled: !paletteSidePanel.findPaletteProcess &&
                                      !paletteSidePanel.posterizationProcess &&
                                      !root.createColoringProcess
        openButton.enabled: createColoringButton.enabled
        settingsButton.enabled: !root.createColoringProcess
        saveButton.enabled: !root.createColoringProcess
        pencilButton.enabled: !root.createColoringProcess
        fillButton.enabled: !root.createColoringProcess
        changeColorButton.enabled: !root.createColoringProcess
        modeComboBox.model: modeComboBoxShortModel
        modeComboBox.visible: root.posterized
        modeComboBox.enabled: !root.createColoringProcess || modeComboBox.model.length < 3
        coloringComboBox.enabled: modeComboBox.enabled
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
                break;
            }
        }

        createColoringButton.onClicked: {
            pageHeader.pencilMode = pageHeader.fillMode = pageHeader.changeColorMode = false;
            colorSidePanel.close();
            pageFooter.busyIndicator.active = true;
            imageProcessor.coloring();
            createColoringProcess = true;
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
            pageFooter.footerText.text = changeColorMode ?
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
                pageFooter.footerText.text = "Укажите пиксель, которому хотите задать выбранный цвет"
            }
            else
                pageFooter.footerText.text = ""
        }
        onFillModeChanged: {
            if(fillMode)
            {
                paletteSidePanel.close()
                colorSidePanel.open();
                pageFooter.footerText.text = "Укажите область, которую хотите залить выбранным цветом"
            }
            else
                pageFooter.footerText.text = ""
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
            width: paletteSidePanel.visible ? paletteSidePanel.width :
                                              colorSidePanel.visible ?
                                              colorSidePanel.width : 0
            PaletteSidePanel{
                id: paletteSidePanel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                visible: false
                enabled: !root.createColoringProcess
                onPosterizeButtonClicked: {
                    posterizationProcess = true;
                    pageFooter.busyIndicator.active = true;
                    imageProcessor.setColors(paletteSidePanel.getColors())
                    imageProcessor.posterize();
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

        Rectangle{
            color: Properties.backgroundColor
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: sidePanelItem.right
            ImageArea{
                id: imageArea
                changeColorMode: root.changeColorMode

                anchors.fill: parent
                onClicked: {
                    if(root.changeColorMode)
                    {
                        root.changeColorX = x;
                        root.changeColorY = y;
                        changeColorDialog.open();
                    }
                    else if(pageHeader.pencilMode)
                    {
                        pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
                        imageProcessor.removeColoringFromProvider();
                        imageArea.source = "";
                        imageProcessor.setPixelColor(x, y, colorSidePanel.currentColor);
                        imageArea.source = "image://provider/posterized";
                    }
                    else if(pageHeader.fillMode)
                    {
                        pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
                        imageProcessor.removeColoringFromProvider();
                        imageArea.source = "";
                        imageProcessor.fill(x, y, colorSidePanel.currentColor);
                        imageArea.source = "image://provider/posterized";
                    }
                }
            }
        }
    }

    PageFooter{
        id: pageFooter
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        footerText.text: "Откройте изображение"
        scaleText.text: Math.floor(imageArea.zoom * 100) + " %";
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
                if(pageHeader.modeComboBox.model.length > 2)
                    pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
                pageHeader.modeComboBox.currentIndex = pageHeader.modeComboBoxOriginalIndex;
                pageHeader.paletteButton.visible = true;
                root.posterized = false;
                pageFooter.footerText.text = "Задайте палитру и постеризуйте изображение"
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

    Dialog{
        id: settingsDialog
        title: "Настройки"
        width: 420
        height: 250
        contentItem: Rectangle {
            color: Properties.backgroundColor
            anchors.fill: parent
            Item {
                anchors.fill: parent
                anchors.margins: 10
                ColumnLayout{
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 5
                    Label{
                        text: "Цвет контуров и цифр"
                        color: "white"
                        font.pointSize: 10
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
                        font.pointSize: 9
                        visible: scalingCheckBox.checked
                        color: "#cccccc"
                        text: "Масштабирование позоляет увеличить конутура,\n" +
                              "сделать их более гладкими и вместить больше числовых меток"
                    }
                    Row{
                        visible: scalingCheckBox.checked
                        width: parent.width
                        spacing: 5
                        Text{
                            font.pointSize: 10
                            color: "white"
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
                    height: 24
                    width: 80
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
    }

    Dialog{
        title: "Сохранение"
        id: saveDialog
        width: 400
        height: 160
        contentItem: Rectangle {
            anchors.fill: parent
            color: Properties.backgroundColor
            Item{
                anchors.fill: parent
                anchors.margins: 10
                Column{
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.left: parent.left
                    spacing: 5
                    Text{
                        font.pointSize: 10
                        color: "white"
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
                            color: "white"
                            implicitHeight: 24
                            background: Rectangle{
                                height: 24
                                border.color: "white"
                                color: "transparent"
                                border.width: 1
                            }
                        }
                        CustomButton{
                            text: "Открыть"
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
                            font.pointSize: 10
                            color: "white"
                            text: "По высоте"
                        }
                        CustomSpinBox{
                            id: rowsTileSpinBox
                            value: 2
                            minimum: columnTileSpinBox.value > 1 ? 1 : 2
                        }
                        Text{
                            font.pointSize: 10
                            color: "white"
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
                    height: 24
                    width: 80
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
    }

    ColorDialog{
        id: changeColorDialog
        title: "Укажите новый цвет"
        onAccepted: {
            imageProcessor.removeColoringFromProvider();
            imageArea.source = "";
            imageProcessor.changeColor(root.changeColorX, root.changeColorY, color);
            imageArea.source = "image://provider/posterized";
            pageHeader.changeColorMode = false;
            pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
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
        let scalingFactor = scalingCheckBox.checked ?
                            scalingSpinBox.value : 1
        configManager.setConfigs(simplify, coloringColor, scalingFactor);
    }

    function applySettings()
    {
        if(pageHeader.modeComboBox.currentIndex == pageHeader.modeComboBoxColoringIndex)
        {
            root.createColoringProcess = true;
            pageFooter.busyIndicator.active = true;
            imageProcessor.coloring();
        }
        else
        {
            pageHeader.modeComboBox.model = pageHeader.modeComboBoxShortModel;
        }
    }
}
