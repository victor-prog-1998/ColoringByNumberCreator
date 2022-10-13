import QtQuick 2.0

Rectangle{
    id: pageFooter
    height: 30
    clip: true
    property alias footerText: footerText
    property alias scaleText: scaleText
    property alias busyIndicator: busyIndicator

    Text{
        id: footerText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pointSize: 11
    }

    CustomBusyIndicator{
        id: busyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: footerText.right
        anchors.leftMargin: 8
    }

    Text{
        id: scaleText
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.margins: 10
        font.pointSize: 9
    }
}
