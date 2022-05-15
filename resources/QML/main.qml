import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
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
        onSaveButtonClicked: saveDialog.open()
        modeComboBox.visible: root.posterized
        modeComboBox.onActivated: { // выбрано пользователем
            switch(index)
            {
            case 0:
                imageArea.source = root.imageOriginalSource;
                break;
            case 1:
                imageArea.source = root.posterizedImageSource;
                break;
            case 2:
                imageProcessor.edges();
                imageArea.source = root.edgesImageSource;
                break;
            case 3:
                imageProcessor.coloring();
                coloringComboBox.currentIndex = 0;
                imageArea.source = root.coloringImageSource;
            }
        }

        coloringComboBox.onActivated: {
            switch(index)
            {
            case 0:
                imageArea.source = root.coloringImageSource;
                break;
            case 1:
                imageArea.source = root.paintedImageSource;
                break;
            case 2:
                imageArea.source = root.legendImageSource;
                break;
            }
        }

        onPaletteButtonClicked: {
            paletteSidePanel.open();
        }

        onPencilModeChanged: {
            if(pencilMode)
            {
                paletteSidePanel.close()
                colorSidePanel.open();
            }
        }
        onFillModeChanged: {
            if(fillMode)
            {
                paletteSidePanel.close()
                colorSidePanel.open();
            }
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
                    footerText.text = "Идет постеризация ..." // не работает
                    imageProcessor.posterize();
                    footerText.text = ""
                    imageArea.source = "";
                    imageArea.source = root.posterizedImageSource
                    root.posterized = true;
                    pageHeader.modeComboBox.currentIndex = 1; // постеризация
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
                pageHeader.modeComboBox.currentIndex = 0; // оригинал
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
                CheckBox{
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


}
