import QtQuick 2.0

Rectangle{
    id: pageFooter
    height: 30
    property alias footerText: footerText
    property alias scaleText: scaleText

    Text{
        id: footerText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pointSize: 11
    }

    Text{
        id: scaleText
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        font.pointSize: 9
    }
}
