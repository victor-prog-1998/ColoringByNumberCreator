import QtQuick 2.0
import "../js/Properties.js" as Properties

Rectangle{
    id: root
    height: 24
    width: 75
    color: Properties.backgroundColor
    border.width: 1
    border.color: "white"
    property int value: 0
    property int minimum: 0
    property int maximum: 100
    property bool fixedValues: false
    property int fixedIndex: 0
    property var values: []

    onFixedValuesChanged: {
        fixedIndex = 0;
        value = values[fixedIndex]
    }

    Text{
        anchors.centerIn: parent
        font.pointSize: 10
        text: root.value
        color: "white"
    }
    CustomButton{
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: height
        buttonColor: Properties.customSpinBoxButtonColor
        hoverColor: Properties.customSpinBoxButtonHoverColor
        text: "-"
        textColor: "white"
        onClicked: {
            if(root.fixedValues)
            {
                if(root.fixedIndex > 0)
                {
                    --root.fixedIndex;
                    root.value = root.values[fixedIndex]
                }
            }

            else if(root.value > root.minimum)
            {
                --root.value;
            }
        }
    }
    CustomButton{
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: height
        buttonColor: Properties.customSpinBoxButtonColor
        hoverColor: Properties.customSpinBoxButtonHoverColor
        text: "+"
        textColor: "white"
        onClicked: {
            if(root.fixedValues)
            {
                if(root.fixedIndex < root.values.length - 1)
                {
                    ++root.fixedIndex;
                    root.value = root.values[fixedIndex]
                }
            }

            else if(root.value < root.maximum)
            {
                ++root.value;
            }
        }
    }

}
