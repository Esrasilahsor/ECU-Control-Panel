import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string moduleId: ""
    property string moduleName: ""
    property bool connected: false

    signal openDataView()
    signal openFirmware()

    radius: 10
    color: "#20252b"

    border.color: connected ? "#42d77d" : "#ef5350"
    border.width: 2

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 35

            Text {
                text: root.moduleName
                color: "white"
                font.pixelSize: 20
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            Rectangle {
                width: 14
                height: 14
                radius: 7

                color: root.connected
                       ? "#42d77d"
                       : "#ef5350"
            }

            Text {
                text: root.connected
                      ? "Connected"
                      : "Disconnected"

                color: root.connected
                       ? "#42d77d"
                       : "#ef5350"

                font.pixelSize: 13
            }
        }

        ComboBox {
            id: modeBox

            Layout.fillWidth: true
            Layout.preferredHeight: 42

            model: [
                "DATA VIEW",
                "FIRMWARE UPLOAD"
            ]

            onActivated: {
                dispatcher.changeMode(
                    root.moduleId,
                    currentText
                )
            }
        }

        Button {
            text: "OPEN " + modeBox.currentText

            Layout.fillWidth: true
            Layout.preferredHeight: 42

            onClicked: {
                if (modeBox.currentText === "DATA VIEW") {
                    root.openDataView()
                }
                else {
                    root.openFirmware()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            spacing: 10

            Button {
                text: "CONNECT"
                Layout.fillWidth: true
                Layout.fillHeight: true

                onClicked: {
                    dispatcher.connectModule(
                        root.moduleId
                    )
                }
            }

            Button {
                text: "DISCONNECT"
                Layout.fillWidth: true
                Layout.fillHeight: true

                onClicked: {
                    dispatcher.disconnectModule(
                        root.moduleId
                    )
                }
            }
        }

        Text {
            text: "POWER"

            color: "#b9c1c9"

            font.pixelSize: 14
            font.bold: true

            Layout.preferredHeight: 20
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            spacing: 10

            Button {
                text: "ON"
                Layout.fillWidth: true
                Layout.fillHeight: true

                onClicked: {
                    dispatcher.powerOn(
                        root.moduleId
                    )
                }
            }

            Button {
                text: "OFF"
                Layout.fillWidth: true
                Layout.fillHeight: true

                onClicked: {
                    dispatcher.powerOff(
                        root.moduleId
                    )
                }
            }

            Button {
                text: "KILL"
                Layout.fillWidth: true
                Layout.fillHeight: true

                onClicked: {
                    dispatcher.killModule(
                        root.moduleId
                    )
                }
            }
        }
    }
}