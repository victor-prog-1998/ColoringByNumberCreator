import QtQuick 2.0

Flickable{
    id: root
    property double zoom: 1.0
    property alias source: img.source
    property bool changeColorMode: false
    signal clicked(var x, var y)

    contentWidth: img.width * zoom
    contentHeight: img.height * zoom
    clip: true

    Image {
        id: img
        cache: false
        smooth: false
        transform: Scale{
            origin.x: 0
            origin.y: 0
            xScale: root.zoom
            yScale: root.zoom
        }

        MouseArea{
            cursorShape: dragging ? Qt.ClosedHandCursor : root.changeColorMode ? Qt.CrossCursor : Qt.ArrowCursor
            anchors.fill: parent
            onWheel: {
                var k = 1.1;
                var scaleFactor = wheel.angleDelta.y > 0 ? k : 1 / k;
                root.zoom *= scaleFactor;
            }
            onClicked: {
                root.clicked(Math.floor(mouseX), Math.floor(mouseY))
            }
        }
    }
}
