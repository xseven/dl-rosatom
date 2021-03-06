import QtQuick 2.12
import QtQuick.Window 2.12
import QtWebSockets 1.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

Window {
    id: root

    visible: true
    width: 640
    height: 480
    title: qsTr("Составитель документов")

    color: "#222831"

    Timer {
        id: socketTimer

        interval: 30000
        running: false
        repeat: true
        onTriggered: {
            socket.active = true
        }
    }

    WebSocket {
            id: socket

            active: true

            url: "ws://127.0.0.1:2748"

            onTextMessageReceived: {
                console.log("Received text message: " + message)
            }

            onBinaryMessageReceived: {
                console.log("Received binary message: " + message)

                ResultsModel.insertEntries(message)
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


    Component {
        id: wrappingDelegate

        RowLayout
        {
            width: parent.width

            Text {
                font.pixelSize: 15
                text: entry
                color: "#ECECEC"
                wrapMode: Text.Wrap
                Layout.fillWidth: true

            }
        }
    }


    GridLayout {
        id: grid

        columns: 2

        anchors.top: parent.top
        height: root.height / 3

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
            var packet = {}
            packet.cmd = "getdata"
            packet.position = position.text
            packet.department = department.text
            packet.keywords = keywords.text

            socket.sendBinaryMessage(JSON.stringify(packet))
          }

          enabled: false
        }
    }

         //[]
      ListView {
          clip: true
          model: ResultsModel
          delegate: wrappingDelegate

          anchors.top: grid.bottom
          anchors.bottom: parent.bottom
          width: root.width

          highlight: Rectangle {
              width: parent.width
              color: "lightgray"
          }
      }
}
