import QtQuick 2.0

Flickable{
    id: root
    property double zoom: 1.0
    property alias source: img.source
    property bool changeColorMode: false
    property int lastContentX: 0
    property int lastContentY: 0

    signal clicked(var x, var y)

    contentWidth: img.width * zoom
    contentHeight: img.height * zoom
    clip: true

   onMovementEnded:{
       lastContentX = contentX;
       lastContentY = contentY;
   }

    Image {
        id: img
        cache: false
        smooth: false
        onSourceChanged: {
            root.contentX = lastContentX;
            root.contentY = lastContentY;
        }

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
                var deltaScale = wheel.angleDelta.y > 0 ? k : 1 / k;
                var newScale = root.zoom * deltaScale;
                if(newScale > 0.1) root.zoom = newScale;
            }
            onClicked: {
                root.clicked(Math.floor(mouseX), Math.floor(mouseY))
            }
        }
    }
}
