import QtQuick 2.0

Rectangle{
    id: root
    height: 30
    width: 100
    color: "white"
    border.width: 1
    property int value: 0
    property int minimum: 0
    property int maximum: 100
    Text{
        anchors.centerIn: parent
        font.pointSize: 12
        text: root.value
    }
    CustomButton{
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: height
        buttonColor: "navy"
        hoverColor: "#0707a4"
        text: "-"
        textColor: "white"
        onClicked: {
            if(root.value > root.minimum)
                --root.value;
        }
    }
    CustomButton{
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: height
        buttonColor: "navy"
        hoverColor: "#0707a4"
        text: "+"
        textColor: "white"
        onClicked: {
            if(root.value < root.maximum)
                ++root.value;
        }
    }

}
