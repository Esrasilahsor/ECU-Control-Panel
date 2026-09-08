import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window

    visible: true
    width: 1200
    height: 800

    minimumWidth: 900
    minimumHeight: 650

    title: "ECU Control Panel"
    color: "#15191e"

    property string selectedModuleId: ""
    property string selectedModuleName: ""

    property int currentPage: 0

    StackLayout {
        anchors.fill: parent

        currentIndex: window.currentPage

        // =========================================
        // PAGE 0 - MAIN CONTROL PANEL
        // =========================================

        Item {
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 14

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70

                    radius: 10
                    color: "#20252b"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 18

                        Text {
                            text: "ECU CONTROL PANEL"

                            color: "white"

                            font.pixelSize: 26
                            font.bold: true
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Rectangle {
                            width: 16
                            height: 16
                            radius: 8

                            color:
                                dispatcher.allConnected
                                ? "#42d77d"
                                : "#ef5350"
                        }

                        Text {
                            text:
                                dispatcher.allConnected
                                ? "SYSTEM CONNECTED"
                                : "SYSTEM DISCONNECTED"

                            color:
                                dispatcher.allConnected
                                ? "#42d77d"
                                : "#ef5350"

                            font.pixelSize: 16
                            font.bold: true
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 500

                    columns: 2
                    rows: 2

                    columnSpacing: 14
                    rowSpacing: 14

                    ECUPanel {
                        moduleId: "A1"
                        moduleName: "ECU A.1"

                        connected:
                            dispatcher.a1Connected

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onOpenDataView: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 1
                        }

                        onOpenFirmware: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 2
                        }
                    }

                    ECUPanel {
                        moduleId: "A2"
                        moduleName: "ECU A.2"

                        connected:
                            dispatcher.a2Connected

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onOpenDataView: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 1
                        }

                        onOpenFirmware: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 2
                        }
                    }

                    ECUPanel {
                        moduleId: "B1"
                        moduleName: "ECU B.1"

                        connected:
                            dispatcher.b1Connected

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onOpenDataView: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 1
                        }

                        onOpenFirmware: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 2
                        }
                    }

                    ECUPanel {
                        moduleId: "B2"
                        moduleName: "ECU B.2"

                        connected:
                            dispatcher.b2Connected

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onOpenDataView: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 1
                        }

                        onOpenFirmware: {
                            window.selectedModuleId =
                                    moduleId

                            window.selectedModuleName =
                                    moduleName

                            window.currentPage = 2
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120

                    radius: 10
                    color: "#20252b"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 6

                        Text {
                            text: "LOG"

                            color: "white"

                            font.bold: true
                            font.pixelSize: 16
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            TextArea {
                                text: dispatcher.log

                                readOnly: true

                                color: "#d7dde3"

                                font.family: "Consolas"
                                font.pixelSize: 13

                                background: Rectangle {
                                    color: "#15191e"
                                    radius: 5
                                }

                                onTextChanged: {
                                    cursorPosition =
                                            length
                                }
                            }
                        }
                    }
                }
            }
        }

        // =========================================
        // PAGE 1 - DATA VIEW
        // =========================================

        DataView {
            moduleId:
                window.selectedModuleId

            moduleName:
                window.selectedModuleName

            onGoBack: {
                window.currentPage = 0
            }
        }

        // =========================================
        // PAGE 2 - FIRMWARE
        // =========================================

        FirmwareUpload {
            moduleId:
                window.selectedModuleId

            moduleName:
                window.selectedModuleName

            onGoBack: {
                window.currentPage = 0
            }
        }
    }
}