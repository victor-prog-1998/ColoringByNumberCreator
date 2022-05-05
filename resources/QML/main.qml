import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    property string imageOriginalSource
    property string posterizedImageSource: "image://provider/posterized"
    property string edgesImageSource: "image://provider/edges"
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
        modeComboBox.visible: root.posterized



        modeComboBox.onActivated: { // выбрано пользователем
            switch(index)
            {
            case 0:
                imageArea.source = root.imageOriginalSource;
                break;
            case 1:
                imageArea.source = root.posterizedImageSource
                break;
            case 2:
                imageProcessor.edges()
                imageArea.source = "image://provider/edges"
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
                    imageProcessor.removeImageFromProvider("edges");
                    root.changeColorX = x;
                    root.changeColorY = y;
                    changeColorDialog.open();
                }
                else if(pageHeader.pencilMode)
                {
                    imageProcessor.removeImageFromProvider("edges");
                    imageArea.source = "";
                    imageProcessor.setPixelColor(x, y, colorSidePanel.currentColor);
                    imageArea.source = "image://provider/posterized";
                }
                else if(pageHeader.fillMode)
                {
                    imageProcessor.removeImageFromProvider("edges");
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
    }

    FileDialog{
        id: fileDialog
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

    MessageDialog{
        id: messageDialog
        visible: false
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
