import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

Rectangle {
    id: root

    property string moduleId: ""
    property string moduleName: ""

    property string firmwarePath: ""
    property string firmwareName: ""

    property int uploadProgress: 0
    property bool uploading: false

    signal goBack()

    color: "#15191e"


    // =====================================================
    // ECU CONNECTION
    // =====================================================

    function ecuConnected()
    {
        if (dispatcher === null)
            return false


        if (root.moduleId === "A1")
            return dispatcher.a1Connected

        if (root.moduleId === "A2")
            return dispatcher.a2Connected

        if (root.moduleId === "B1")
            return dispatcher.b1Connected

        if (root.moduleId === "B2")
            return dispatcher.b2Connected


        return false
    }


    // =====================================================
    // FIRMWARE STATE
    // =====================================================

    function updateFirmwareState()
    {
        if (dispatcher === null ||
            root.moduleId === "")
        {
            root.uploadProgress = 0
            root.uploading = false

            return
        }


        root.uploadProgress =
                dispatcher.firmwareProgress(
                    root.moduleId
                )


        root.uploading =
                dispatcher.isFirmwareUploading(
                    root.moduleId
                )
    }


    // =====================================================
    // BYTES FORMAT
    // =====================================================

    function formatBytes(bytes)
    {
        var value =
                Number(bytes)


        if (isNaN(value))
            return "0 B"


        if (value < 1024)
        {
            return value
                    + " B"
        }


        if (value < 1024 * 1024)
        {
            return (
                value / 1024
            ).toFixed(2)
                    + " KB"
        }


        return (
            value
            /
            (
                1024
                *
                1024
            )
        ).toFixed(2)
                + " MB"
    }


    // =====================================================
    // COMPONENT
    // =====================================================

    Component.onCompleted: {

        root.updateFirmwareState()
    }


    onModuleIdChanged: {

        Qt.callLater(
            root.updateFirmwareState
        )
    }


    onVisibleChanged: {

        if (visible)
        {
            root.updateFirmwareState()
        }
    }


    // =====================================================
    // DISPATCHER SIGNALS
    // =====================================================

    Connections {

        target: dispatcher


        onFirmwareStateChanged: {

            root.updateFirmwareState()
        }


        onFirmwareMessage: {

            if (moduleId !== root.moduleId)
                return


            root.updateFirmwareState()


            statusText.text =
                    message


            statusText.color =
                    success
                    ? "#42d77d"
                    : "#ef5350"
        }
    }


    // =====================================================
    // FILE DIALOG
    // =====================================================

    FileDialog {

        id: firmwareDialog


        title:
            "Select Firmware for "
            + root.moduleName


        nameFilters: [
            "Firmware Files (*.bin *.hex)",
            "BIN Firmware (*.bin)",
            "HEX Firmware (*.hex)"
        ]


        onAccepted: {

            root.firmwarePath =
                    fileUrl.toString()


            firmwarePathField.text =
                    root.firmwarePath


            var pathParts =
                    root.firmwarePath.split("/")


            root.firmwareName =
                    pathParts[
                        pathParts.length - 1
                    ]


            fileNameValue.text =
                    root.firmwareName


            /*
                QML FileDialog dosya boyutunu
                doğrudan vermediği için gerçek
                boyut upload başladıktan sonra
                Connector tarafında okunuyor.

                Şimdilik kullanıcıya dosya seçildi
                bilgisini gösteriyoruz.
            */


            statusText.text =
                    "Firmware selected."


            statusText.color =
                    "#b9c1c9"
        }
    }


    // =====================================================
    // MAIN LAYOUT
    // =====================================================

    ColumnLayout {

        anchors.fill: parent

        anchors.margins: 20

        spacing: 14


        // =================================================
        // HEADER
        // =================================================

        RowLayout {

            Layout.fillWidth: true

            Layout.preferredHeight: 55


            Button {

                text: "< BACK"


                onClicked: {

                    /*
                        Upload arka planda devam eder.
                    */

                    root.goBack()
                }
            }


            Text {

                text:
                    root.moduleName
                    + " - FIRMWARE UPLOAD"


                color: "white"


                font.pixelSize: 24

                font.bold: true
            }


            Item {
                Layout.fillWidth: true
            }


            Rectangle {

                width: 14
                height: 14
                radius: 7


                color:
                    root.uploading
                    ? "#42d77d"
                    : "#ef5350"
            }


            Text {

                text:
                    root.uploading
                    ? "UPLOADING"
                    : "IDLE"


                color:
                    root.uploading
                    ? "#42d77d"
                    : "#b9c1c9"


                font.bold: true
            }
        }


        // =================================================
        // CONNECTION
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 65

            radius: 10

            color: "#20252b"


            RowLayout {

                anchors.fill: parent

                anchors.margins: 15


                Text {

                    text:
                        "Target ECU: "
                        + root.moduleName


                    color: "white"

                    font.bold: true
                }


                Item {
                    Layout.fillWidth: true
                }


                Rectangle {

                    width: 14
                    height: 14
                    radius: 7


                    color:
                        root.ecuConnected()
                        ? "#42d77d"
                        : "#ef5350"
                }


                Text {

                    text:
                        root.ecuConnected()
                        ? "CONNECTED"
                        : "DISCONNECTED"


                    color:
                        root.ecuConnected()
                        ? "#42d77d"
                        : "#ef5350"


                    font.bold: true
                }
            }
        }


        // =================================================
        // FILE SELECT
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 120

            radius: 10

            color: "#20252b"


            ColumnLayout {

                anchors.fill: parent

                anchors.margins: 15

                spacing: 10


                Text {

                    text: "FIRMWARE FILE"

                    color: "white"

                    font.bold: true

                    font.pixelSize: 16
                }


                RowLayout {

                    Layout.fillWidth: true


                    TextField {

                        id: firmwarePathField


                        Layout.fillWidth: true


                        readOnly: true


                        placeholderText:
                            "Select .bin or .hex firmware"
                    }


                    Button {

                        text: "SELECT FIRMWARE"


                        enabled:
                            !root.uploading


                        onClicked: {

                            firmwareDialog.open()
                        }
                    }
                }
            }
        }


        // =================================================
        // FILE INFORMATION
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 120

            radius: 10

            color: "#20252b"


            GridLayout {

                anchors.fill: parent

                anchors.margins: 15

                columns: 2

                rowSpacing: 8

                columnSpacing: 20


                Text {
                    text: "File Name:"
                    color: "#b9c1c9"
                }


                Text {

                    id: fileNameValue

                    text:
                        root.firmwareName !== ""
                        ? root.firmwareName
                        : "-"

                    color: "white"

                    font.bold: true
                }


                Text {
                    text: "Format:"
                    color: "#b9c1c9"
                }


                Text {

                    text: {

                        if (root.firmwareName === "")
                            return "-"


                        var parts =
                                root.firmwareName.split(".")


                        if (parts.length < 2)
                            return "-"


                        return "."
                                + parts[
                                    parts.length - 1
                                ].toUpperCase()
                    }

                    color: "white"

                    font.bold: true
                }


                Text {
                    text: "Chunk Size:"
                    color: "#b9c1c9"
                }


                Text {

                    text:
                        "256 bytes"

                    color: "white"

                    font.bold: true
                }
            }
        }


        // =================================================
        // UPLOAD
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 250

            radius: 10

            color: "#20252b"


            ColumnLayout {

                anchors.fill: parent

                anchors.margins: 20

                spacing: 15


                Text {

                    text: "UPLOAD PROGRESS"

                    color: "white"

                    font.bold: true

                    font.pixelSize: 16
                }


                ProgressBar {

                    Layout.fillWidth: true

                    from: 0

                    to: 100

                    value:
                        root.uploadProgress
                }


                Text {

                    text:
                        root.uploadProgress
                        + " %"


                    color: "white"


                    font.pixelSize: 32

                    font.bold: true


                    Layout.alignment:
                        Qt.AlignHCenter
                }


                RowLayout {

                    Layout.alignment:
                        Qt.AlignHCenter


                    Button {

                        text: "START UPLOAD"


                        enabled:
                            !root.uploading


                        onClicked: {

                            if (!root.ecuConnected())
                            {
                                statusText.text =
                                        "ECU must be connected first."


                                statusText.color =
                                        "#ef5350"


                                return
                            }


                            if (root.firmwarePath === "")
                            {
                                statusText.text =
                                        "Select firmware file first."


                                statusText.color =
                                        "#ef5350"


                                return
                            }


                            dispatcher.startFirmwareUpload(
                                root.moduleId,
                                root.firmwarePath
                            )


                            root.updateFirmwareState()


                            statusText.text =
                                    "Processing firmware chunks..."


                            statusText.color =
                                    "#42d77d"
                        }
                    }


                    Button {

                        text: "CANCEL"


                        enabled:
                            root.uploading


                        onClicked: {

                            dispatcher.cancelFirmwareUpload(
                                root.moduleId
                            )


                            root.updateFirmwareState()
                        }
                    }
                }


                Text {

                    id: statusText


                    text:
                        "Select a firmware file."


                    color: "#b9c1c9"


                    Layout.alignment:
                        Qt.AlignHCenter
                }
            }
        }


        // =================================================
        // EXPLANATION
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.fillHeight: true

            radius: 10

            color: "#20252b"


            ColumnLayout {

                anchors.fill: parent

                anchors.margins: 15

                spacing: 8


                Text {

                    text:
                        "SIMULATION INFORMATION"

                    color: "white"

                    font.bold: true
                }


                Text {

                    text:
                        "The firmware file is read in 256-byte chunks."

                    color: "#b9c1c9"
                }


                Text {

                    text:
                        "Progress is calculated from processed bytes / total file size."

                    color: "#b9c1c9"
                }


                Text {

                    text:
                        "No real CAN firmware transmission is performed yet."

                    color: "#b9c1c9"
                }
            }
        }
    }
}