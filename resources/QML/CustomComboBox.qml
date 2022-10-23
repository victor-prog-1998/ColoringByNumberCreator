import QtQuick 2.10
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    property var model: []
    property int currentIndex: 0
    property bool down: false
    signal activated(var index)

    MouseArea{
        anchors.fill: parent
        onClicked: {
            root.down = !root.down;
        }
    }

    height: 24
    width: 150
    border.color: "white"
    color: Properties.headerColor
    Text{
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 4
        text: root.model[root.currentIndex]
        color: "white"
    }
    Canvas{
        id: indicator
        width: 12
        height: 8
        rotation: root.down ? 180 : 0
        contextType: "2d"
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 4
        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.strokeStyle = "white"
            context.stroke();
        }
        Behavior on rotation {
            NumberAnimation { easing.type: Easing.InQuad;duration: 150}
        }
    }

    Popup{
        visible: root.down
        x: 0
        y: root.height - 1
        padding: 1
        implicitHeight: contentItem.implicitHeight
        width: root.width
        contentItem: ListView{
            model: root.down ? root.model : null
            ScrollIndicator.vertical: ScrollIndicator { }
            implicitHeight: contentHeight
            delegate: Rectangle{
                width: root.width
                height: 24
                color: listElementMa.containsMouse ? Properties.comboBoxHighlightColor :
                       index == root.currentIndex ? Properties.comboBoxSelectedColor :
                       Properties.comboBoxColor;
                border.color: "darkgray"
                border.width: 1
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    text: modelData
                    color: "white"
                }
                MouseArea{
                    id: listElementMa
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        root.currentIndex = index;
                        root.activated(index)
                        root.down = false;
                    }
                }
            }
        }

        background: Rectangle{
            border.width: 1
            border.color: "white"
            color: Properties.headerColor
        }
    }
}

//ComboBox {
//        id: root
//        currentIndex: -1
//        rightPadding: 6
//        background: Rectangle{
//            color: Properties.headerColor
//            border.width: 1
//            border.color: "white"
//        }

//        indicator: Canvas {
//                id: canvas
//                x: root.width - width - root.rightPadding
//                y: root.topPadding + (root.availableHeight - height) / 2
//                width: 12
//                height: 8
//                contextType: "2d"
//                rotation: root.popup.visible ? 180 : 0

//                Connections {
//                    target: root
//                    function onPressedChanged() { canvas.requestPaint(); }
//                }

//                onPaint: {
//                    context.reset();
//                    context.moveTo(0, 0);
//                    context.lineTo(width, 0);
//                    context.lineTo(width / 2, height);
//                    context.closePath();
//                    context.strokeStyle = "white"
//                    context.stroke();
//                }
//        }


//        contentItem: Text {
//                leftPadding: 10
//                rightPadding: root.indicator.width + root.spacing
//                text: root.displayText
//                font: root.font
//                color: Properties.headerTextColor
//                verticalAlignment: Text.AlignVCenter
//                elide: Text.ElideRight
//            }

//        delegate: ItemDelegate {
//                width: root.width
//                implicitHeight: 32
//                contentItem: Text {
//                    text: modelData
//                    font: root.font
//                    color: "white"
//                    elide: Text.ElideRight
//                    verticalAlignment: Text.AlignVCenter
//                }
//                highlighted: root.highlightedIndex === index
//            }

//        popup: Popup {
//                y: root.height - 1
//                width: root.width
//                implicitHeight: contentItem.implicitHeight
//                padding: 1
//                contentItem: ListView {
//                    clip: true
//                    implicitHeight: contentHeight
//                    model: root.popup.visible ? root.delegateModel : null
//                    currentIndex: root.highlightedIndex
//                    ScrollIndicator.vertical: ScrollIndicator { }
//                }
//                background: Rectangle {
//                    color: Properties.headerColor
//                    border.color: "white"
//                    radius: 2
//                }
//            }
//    }
