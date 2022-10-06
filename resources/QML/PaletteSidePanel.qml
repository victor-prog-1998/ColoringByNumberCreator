import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.3

SidePanel{
    id: root
    property alias colorsModel: colorsModel
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

        Rectangle{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: buttonsColumn.top
            anchors.margins: 3
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
            anchors.margins: 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            ColorCountPopup{
                id: colorsCountPopup
                x: colorPalleteButton.x
                y: colorPalleteButton.y - height
                onAccepted: {
                    imageProcessor.findOptimalPalette(value);
                    colorsModel.clear();
                }
            }
            CustomButton{
                id: colorPalleteButton
                width: parent.width
                border.width: 1
                text: "Рассчитать палитру"
                onClicked: colorsCountPopup.open()
            }
            CustomButton{
                id: addColorButton
                width: parent.width
                border.width: 1
                text: "Добавить цвет"
                onClicked: {
                    colorDialog.open();
                }
            }
            CustomButton{
                border.width: 1
                width: parent.width
                text: "Постеризовать"
                onClicked: root.posterizeButtonClicked()
                enabled: colorsModel.count > 1
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
    }
}
