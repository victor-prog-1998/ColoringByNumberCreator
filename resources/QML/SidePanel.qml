import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    width: 200
    border.width: 1
    color: Properties.sidePanelColor

    property int minimumWidth: 100
    property int maximumWidth: 300
    property alias resizeRect: resizeRect
    property alias title: txt.text
    property alias header: header
    signal closed()

    function close()
    {
        root.closed();
        root.visible = false;
    }

    function open()
    {
        root.visible = true;
    }

    Item{
        id: header
        anchors.top: parent.top
        anchors.right: resizeRect.left
        anchors.left: parent.left
        anchors.topMargin: 2
        anchors.leftMargin: 6
        anchors.rightMargin: 2
        height: 16
        Text{
            id: txt
            anchors.top: parent.top
            anchors.left: parent.left
            font.pointSize: 10
            color: "white"
        }

        ImageButton{
            id: closeButton
            border.width: 0
            anchors.top: parent.top
            anchors.right: parent.right
            source: "qrc:/icons/close-icon.png"
            toolTipText: "Закрыть панель"
            onClicked: {
                root.close();
            }
        }
    }

    Rectangle{
        id: resizeRect
        x: parent.width - width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 3
        color: "transparent"

        Drag.active: resizerMa.drag.active
        onXChanged: {
            root.width = x + width
        }

        MouseArea{
            id: resizerMa
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: root.minimumWidth
            drag.maximumX: root.maximumWidth
            anchors.fill: parent
            cursorShape: Qt.SplitHCursor
        }
    }
}
