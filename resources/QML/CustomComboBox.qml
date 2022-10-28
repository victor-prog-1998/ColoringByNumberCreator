import QtQuick 2.10
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    property var model: []
    property int currentIndex: 0
    property bool down: false
    signal activated(var index)
    opacity: enabled ? 1.0 : 0.7
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
