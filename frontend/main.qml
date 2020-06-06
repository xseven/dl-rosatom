 import QtQuick 2.12
import QtQuick.Window 2.12
import QtWebSockets 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

Window {
    id: root

    visible: true
    width: 640
    height: 480
    title: qsTr("Составитель документов")

    color: "#222831"

    signal qmlSignal(msg: string)

    Timer {
        id: socketTimer

        interval: 30000
        running: false
        repeat: true
        onTriggered: {
            socket.active = true
        }
    }

    RowLayout {

    GridLayout {
          id: grid
          columns: 2

          //[0]
          Text {
              text: qsTr("Ваша должность:");
              color: "#ECECEC"
              font.bold: true;
          }

          TextEdit {
              id: position

              text: "Зоотехник"
              color: "#ECECEC"
              focus: true
          }

          //[1]
          Text {
              text: qsTr("Подразделение:");
              color: "#ECECEC"
              font.bold: true;
          }

          TextEdit {
              id: department

              text: "Ветеринарная служба"
              color: "#ECECEC"
              focus: true
          }

          //[2]
          Text {
              text: qsTr("Ключевые слова (разделенные запятой):");
              color: "#ECECEC"
              font.bold: true;
          }

          TextEdit {
              id: keywords

              text: "Уход за животными, ветеринария, кормление"
              color: "#ECECEC"
              focus: true
          }

          //[3]
          Button {
            id: sendButton

            text: qsTr("Сформировать")
            onClicked: {
                console.log(position.text + "," + department.text + "," + keywords.text)
                socket.sendTextMessage(position.text + "," + department.text + "," + keywords.text)
            }

            enabled: false
          }
        }

    WebSocket {
            id: socket

            active: true

            url: "ws://127.0.0.1:2748"

            onTextMessageReceived: {
                messageBox.text = messageBox.text + "\nReceived message: " + message
            }            
        }

    Connections {
            target: socket

            function reconnect() {
                console.log("Reconnecting in 30 sec")
                socket.active = false
                sendButton.enabled = false
                socketTimer.start()
            }

            function onStatusChanged(status) {
                if(socket.status === WebSocket.Open) {
                    console.log("Connected")
                    sendButton.enabled = true
                } else if (socket.status ==+ WebSocket.Closed) {
                    console.log("Connection closed")
                    reconnect()
                } else if(socket.status ==+ WebSocket.Error) {
                    console.error("error: " + socket.errorString)                    
                } else if(socket.status == WebSocket.Connecting) {
                    console.log("Connecting to " + socket.url)
                }
            }
        }

    ListModel {
        id: nameModel
        ListElement { name: "Alice" }
        ListElement { name: "Bob" }
        ListElement { name: "Jane" }
        ListElement { name: "Harry" }
        ListElement { name: "Wendy" }
    }
    Component {
        id: nameDelegate
        Text {
            text: name;
            font.pixelSize: 24
        }
    }

    ListView {
        clip: true
        model: nameModel
        delegate: nameDelegate
        header: bannercomponent
        footer: Rectangle {
            width: parent.width; height: 30;
            gradient: clubcolors
        }
        highlight: Rectangle {
            width: parent.width
            color: "lightgray"
        }
    }

    Component {     //instantiated when header is processed
        id: bannercomponent
        Rectangle {
            id: banner
            width: parent.width; height: 50
            gradient: clubcolors
            border {color: "#9EDDF2"; width: 2}
            Text {
                anchors.centerIn: parent
                text: "Club Members"
                font.pixelSize: 32
            }
        }
    }
    Gradient {
        id: clubcolors
        GradientStop { position: 0.0; color: "#8EE2FE"}
        GradientStop { position: 0.66; color: "#7ED2EE"}
    }
    }
}
