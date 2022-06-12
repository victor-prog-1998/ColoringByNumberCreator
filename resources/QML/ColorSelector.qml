import QtQuick 2.0
import QtQuick.Layouts 1.12

Item {
    id: root
    Layout.preferredHeight: 40

    property int currentIndex: 0

    function setCurrentColor(colorName)
    {
        for(let i = 0; i < colorsModel.count; ++i)
        {
            let col = colorsModel.get(i)._color;
            if(col === colorName)
            {
                root.currentIndex = i;
                return;
            }
        }
        root.currentIndex = 0;
    }

    function getCurrentColor()
    {
        return colorsModel.get(root.currentIndex)._color;
    }

    RowLayout{
        spacing: 10
        Repeater{
            model: colorsModel
            delegate: Rectangle{
                height: 32
                width: 32
                color: _color
                border.width: index == root.currentIndex ? 4 : 1
                border.color: index == root.currentIndex ? "#238df1" : "black"
                MouseArea{
                    anchors.fill: parent
                    onClicked: root.currentIndex = index;
                }
            }
        }
    }
    ListModel{
        id: colorsModel
    }
    Component.onCompleted: {
        colorsModel.append({_color:"#000000"});
        colorsModel.append({_color:"#2e2e2e"});
        colorsModel.append({_color:"#4a4a4a"});
        colorsModel.append({_color:"#616161"});
        colorsModel.append({_color:"#6b6b6b"});
    }
}
