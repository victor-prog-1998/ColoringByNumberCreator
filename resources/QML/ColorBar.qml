import QtQuick 2.0

Rectangle{
    id: root
    border.width: 1
    border.color: "white"
    width: 40;
    height: width
    property bool small: false
    signal deleteClicked()
    ImageButton{
        anchors.right: parent.right
        anchors.top: parent.top
        source: "qrc:/icons/delete-icon.png"
        onClicked: root.deleteClicked();
        toolTipText: "Удалить"
    }
}
