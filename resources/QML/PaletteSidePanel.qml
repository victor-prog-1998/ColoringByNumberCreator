import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

SidePanel{
    id: root
    property alias colorsModel: colorsModel
    property bool posterizationProcess: false
    property bool findPaletteProcess: false
    signal posterizeButtonClicked()
    minimumWidth: 200
    title: "Палитра"
    function getColors()
    {
        let colors = [];
        for(var i = 0; i < colorsModel.count; ++i)
            colors.push(colorsModel.get(i)._color)
        return colors;
    }
    function setPalette(palette)
    {
        colorsModel.clear();
        for(var i = 0; i < palette.length; ++i)
        {
            let obj = {};
            obj._color = palette[i];
            colorsModel.append(obj);
        }
    }

    Item{
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: resizeRect.left
        anchors.topMargin: 4
        anchors.bottomMargin: 4
        anchors.rightMargin: 4
        anchors.leftMargin: 6

        Rectangle{
            color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: buttonsColumn.top
            anchors.bottomMargin: 3
            border.width: 1
            ScrollView{
                anchors.fill: parent
                anchors.margins: 3
                contentHeight: paletteFlow.implicitHeight
                contentWidth: width
                clip: true
                Flow{
                    id: paletteFlow
                    width: parent.width
                    spacing: 2
                    Repeater{
                        model: colorsModel
                        delegate: ColorBar{
                            color: _color
                            onDeleteClicked: {
                                colorsModel.remove(index)
                            }
                        }
                    }
                }
            }
        }

        Column{
            id: buttonsColumn
            spacing: 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            ColorCountPopup{
                id: colorsCountPopup
                x: calcPaletteButton.x
                y: calcPaletteButton.y - height
                width: parent.width
                onAccepted: {
                    root.findPaletteProcess = true;
                    imageProcessor.findOptimalPalette(value);
                    pageFooter.busyIndicator.active = true;
                    colorsModel.clear();
                }
            }
            CustomButton{
                id: calcPaletteButton
                width: parent.width
                text: "Рассчитать палитру"
                enabled: !root.findPaletteProcess && !root.posterizationProcess
                onClicked: colorsCountPopup.open()
            }
            CustomButton{
                id: addColorButton
                width: parent.width
                text: "Добавить цвет"
                enabled: !root.findPaletteProcess && !root.posterizationProcess
                onClicked: {
                    colorDialog.open();
                }
            }
            CustomButton{
                width: parent.width
                text: "Постеризовать"
                onClicked: root.posterizeButtonClicked()
                enabled: !root.posterizationProcess && colorsModel.count > 1
            }
        }
    }

    ColorDialog{
        id: colorDialog
        onAccepted: {
            var newColor = {};
            newColor._color = color.toString()
            colorsModel.append(newColor)
        }
    }

    ListModel{
        id: colorsModel
        onDataChanged: {
            console.log("Data changed")
        }
    }
}
