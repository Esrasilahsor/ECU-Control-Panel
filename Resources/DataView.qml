import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import Qt.labs.settings 1.1

Rectangle {
    id: root

    property string moduleId: ""
    property string moduleName: ""

    property bool excelSelected: false
    property bool dataRunning: false

    property var parameterNames: []
    property var graphHistory: []

    property string selectedParameter: ""

    signal goBack()

    color: "#15191e"


    // =====================================================
    // CURRENT ECU DATA
    // =====================================================

    property var currentParsedData: {

        if (dispatcher === null)
            return []

        if (root.moduleId === "A1")
            return dispatcher.a1ParsedData

        if (root.moduleId === "A2")
            return dispatcher.a2ParsedData

        if (root.moduleId === "B1")
            return dispatcher.b1ParsedData

        if (root.moduleId === "B2")
            return dispatcher.b2ParsedData

        return []
    }


    // =====================================================
    // SETTINGS
    // =====================================================

    Settings {
        id: appSettings

        category: "ECUExcelFiles"

        property string lastExcelA1: ""
        property string lastExcelA2: ""
        property string lastExcelB1: ""
        property string lastExcelB2: ""

        property string graphParameterA1: ""
        property string graphParameterA2: ""
        property string graphParameterB1: ""
        property string graphParameterB2: ""
    }


    // =====================================================
    // LAST EXCEL PATH
    // =====================================================

    function lastExcelPath()
    {
        if (root.moduleId === "A1")
            return appSettings.lastExcelA1

        if (root.moduleId === "A2")
            return appSettings.lastExcelA2

        if (root.moduleId === "B1")
            return appSettings.lastExcelB1

        if (root.moduleId === "B2")
            return appSettings.lastExcelB2

        return ""
    }


    // =====================================================
    // SAVE EXCEL PATH
    // =====================================================

    function saveExcelPath(path)
    {
        if (root.moduleId === "A1")
            appSettings.lastExcelA1 = path

        else if (root.moduleId === "A2")
            appSettings.lastExcelA2 = path

        else if (root.moduleId === "B1")
            appSettings.lastExcelB1 = path

        else if (root.moduleId === "B2")
            appSettings.lastExcelB2 = path
    }


    // =====================================================
    // SAVED GRAPH PARAMETER
    // =====================================================

    function savedGraphParameter()
    {
        if (root.moduleId === "A1")
            return appSettings.graphParameterA1

        if (root.moduleId === "A2")
            return appSettings.graphParameterA2

        if (root.moduleId === "B1")
            return appSettings.graphParameterB1

        if (root.moduleId === "B2")
            return appSettings.graphParameterB2

        return ""
    }


    // =====================================================
    // SAVE GRAPH PARAMETER
    // =====================================================

    function saveGraphParameter(parameter)
    {
        if (root.moduleId === "A1")
            appSettings.graphParameterA1 = parameter

        else if (root.moduleId === "A2")
            appSettings.graphParameterA2 = parameter

        else if (root.moduleId === "B1")
            appSettings.graphParameterB1 = parameter

        else if (root.moduleId === "B2")
            appSettings.graphParameterB2 = parameter
    }


    // =====================================================
    // RUNNING STATE
    // =====================================================

    function updateRunningState()
    {
        if (dispatcher === null ||
            root.moduleId === "")
        {
            root.dataRunning = false
            return
        }


        root.dataRunning =
                dispatcher.isDataRunning(
                    root.moduleId
                )
    }


    // =====================================================
    // CONNECTION
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
    // FORMAT VALUE
    // =====================================================

    function formatValue(value)
    {
        var number =
                Number(value)


        if (isNaN(number))
            return value


        if (Math.floor(number) === number)
            return number.toString()


        return number.toFixed(2)
    }


    // =====================================================
    // UPDATE PARAMETER NAMES
    // =====================================================

    function updateParameterNames()
    {
        var data =
                root.currentParsedData


        if (!data ||
            data.length === 0)
        {
            return
        }


        var names = []


        for (var i = 0;
             i < data.length;
             i++)
        {
            var name =
                    data[i].name


            if (name === undefined ||
                name === "")
            {
                continue
            }


            if (names.indexOf(name) === -1)
            {
                names.push(name)
            }
        }


        root.parameterNames =
                names


        var saved =
                root.savedGraphParameter()


        if (saved !== "" &&
            names.indexOf(saved) !== -1)
        {
            root.selectedParameter =
                    saved
        }
        else if (names.length > 0)
        {
            root.selectedParameter =
                    names[0]


            root.saveGraphParameter(
                root.selectedParameter
            )
        }


        var position =
                names.indexOf(
                    root.selectedParameter
                )


        if (position >= 0)
        {
            parameterBox.currentIndex =
                    position
        }
    }


    // =====================================================
    // REFRESH GRAPH FROM DISPATCHER
    // =====================================================

    function refreshGraph()
    {
        if (dispatcher === null)
            return


        if (root.moduleId === "" ||
            root.selectedParameter === "")
        {
            root.graphHistory = []


            graphCanvas.requestPaint()

            return
        }


        root.graphHistory =
                dispatcher.graphData(
                    root.moduleId,
                    root.selectedParameter
                )


        graphCanvas.requestPaint()
    }


    // =====================================================
    // LOAD LAST EXCEL
    // =====================================================

    function loadLastExcel()
    {
        if (dispatcher === null)
            return


        var path =
                root.lastExcelPath()


        if (path === "")
        {
            root.excelSelected =
                    false


            excelPath.text =
                    ""


            statusText.text =
                    "Select Excel for "
                    + root.moduleName


            statusText.color =
                    "#b9c1c9"


            return
        }


        excelPath.text =
                path


        var success =
                dispatcher.loadExcel(
                    root.moduleId,
                    path
                )


        if (success)
        {
            root.excelSelected =
                    true


            statusText.text =
                    "Last Excel loaded for "
                    + root.moduleName


            statusText.color =
                    "#42d77d"
        }
        else
        {
            root.excelSelected =
                    false


            statusText.text =
                    "Last Excel could not be loaded."


            statusText.color =
                    "#ef5350"
        }
    }


    // =====================================================
    // INITIALIZE MODULE
    // =====================================================

    function initializeModule()
    {
        if (root.moduleId === "")
            return


        root.parameterNames = []


        root.selectedParameter =
                root.savedGraphParameter()


        /*
            graphHistory burada SIFIRLANMIYOR.

            Dispatcher'da arka planda tutulan
            history refreshGraph() ile geri alınır.
        */


        root.loadLastExcel()


        if (root.currentParsedData &&
            root.currentParsedData.length > 0)
        {
            root.updateParameterNames()
        }


        root.updateRunningState()


        root.refreshGraph()
    }


    // =====================================================
    // MODULE CHANGED
    // =====================================================

    onModuleIdChanged: {

        /*
            STOP YOK.

            Eski ECU Dispatcher'da
            arka planda çalışmaya devam eder.
        */

        Qt.callLater(
            root.initializeModule
        )
    }


    // =====================================================
    // COMPONENT READY
    // =====================================================

    Component.onCompleted: {

        Qt.callLater(
            root.initializeModule
        )
    }


    // =====================================================
    // DISPATCHER SIGNALS
    // =====================================================

    Connections {

        target: dispatcher


        onParsedDataChanged: {

            /*
                Dispatcher'da herhangi bir ECU'dan
                data geldiğinde burası çalışır.

                Ama currentParsedData ve graphData()
                sadece mevcut moduleId'yi kullanır.
            */

            root.updateParameterNames()

            root.refreshGraph()
        }


        onDataRunningChanged: {

            root.updateRunningState()

            root.refreshGraph()
        }


        onConnectionStatesChanged: {

            root.updateRunningState()
        }
    }


    // =====================================================
    // FILE DIALOG
    // =====================================================

    FileDialog {

        id: excelDialog


        title:
            "Select Excel for "
            + root.moduleName


        nameFilters: [
            "Excel Files (*.xlsx)"
        ]


        onAccepted: {

            if (dispatcher === null)
                return


            var selectedPath =
                    fileUrl.toString()


            excelPath.text =
                    selectedPath


            statusText.text =
                    "Loading Excel..."


            statusText.color =
                    "#b9c1c9"


            var success =
                    dispatcher.loadExcel(
                        root.moduleId,
                        selectedPath
                    )


            if (success)
            {
                root.excelSelected =
                        true


                root.saveExcelPath(
                    selectedPath
                )


                /*
                    Yeni Excel seçilince QML tarafındaki
                    parametre listesi yeniden oluşturulacak.

                    Ancak sadece SELECT EXCEL yaptığın için
                    mevcut history otomatik silinmiyor.
                */

                root.parameterNames = []

                root.selectedParameter = ""

                root.saveGraphParameter("")


                root.graphHistory = []


                graphCanvas.requestPaint()


                statusText.text =
                        "Excel loaded successfully for "
                        + root.moduleName


                statusText.color =
                        "#42d77d"
            }
            else
            {
                root.excelSelected =
                        false


                statusText.text =
                        "Excel could not be loaded."


                statusText.color =
                        "#ef5350"
            }
        }
    }


    // =====================================================
    // PAGE VISIBILITY
    // =====================================================

    onVisibleChanged: {

        /*
            Sayfa gizlenince STOP YOK.

            Sadece tekrar görünür olduğunda
            Dispatcher'da biriken son history alınır.
        */

        if (visible)
        {
            root.updateRunningState()

            root.refreshGraph()
        }
    }


    // =====================================================
    // MAIN LAYOUT
    // =====================================================

    ColumnLayout {

        anchors.fill: parent

        anchors.margins: 16

        spacing: 12


        // =================================================
        // HEADER
        // =================================================

        RowLayout {

            Layout.fillWidth: true

            Layout.preferredHeight: 50


            // =============================================
            // BACK
            // =============================================

            Button {

                text: "< BACK"


                onClicked: {

                    /*
                        Burada STOP YOK.

                        Veri Dispatcher'da
                        arka planda devam eder.
                    */

                    root.goBack()
                }
            }


            Text {

                text:
                    root.moduleName
                    + " - DATA VIEW"


                color: "white"


                font.pixelSize: 24

                font.bold: true
            }


            Item {
                Layout.fillWidth: true
            }


            // =============================================
            // STATUS LED
            // =============================================

            Rectangle {

                width: 14

                height: 14

                radius: 7


                color:
                    root.dataRunning
                    ? "#42d77d"
                    : "#ef5350"
            }


            Text {

                text:
                    root.dataRunning
                    ? "DATA RUNNING"
                    : "DATA STOPPED"


                color:
                    root.dataRunning
                    ? "#42d77d"
                    : "#ef5350"


                font.bold: true
            }
        }


        // =================================================
        // CONTROL AREA
        // =================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 115

            radius: 10

            color: "#20252b"


            ColumnLayout {

                anchors.fill: parent

                anchors.margins: 12

                spacing: 8


                RowLayout {

                    Layout.fillWidth: true


                    TextField {

                        id: excelPath


                        Layout.fillWidth: true


                        readOnly: true


                        placeholderText:
                            "Select Excel for "
                            + root.moduleName
                    }


                    // =====================================
                    // SELECT EXCEL
                    // =====================================

                    Button {

                        text: "SELECT EXCEL"


                        onClicked: {

                            excelDialog.open()
                        }
                    }


                    // =====================================
                    // START
                    // =====================================

                    Button {

                        text: "START DATA"


                        enabled:
                            !root.dataRunning


                        onClicked: {

                            if (!root.excelSelected)
                            {
                                statusText.text =
                                        "Select Excel first."


                                statusText.color =
                                        "#ef5350"


                                return
                            }


                            if (!root.ecuConnected())
                            {
                                statusText.text =
                                        "ECU is not connected."


                                statusText.color =
                                        "#ef5350"


                                return
                            }


                            dispatcher.startData(
                                root.moduleId
                            )


                            root.updateRunningState()


                            statusText.text =
                                    "Background live data started for "
                                    + root.moduleName


                            statusText.color =
                                    "#42d77d"


                            root.refreshGraph()
                        }
                    }


                    // =====================================
                    // STOP
                    // =====================================

                    Button {

                        text: "STOP DATA"


                        enabled:
                            root.dataRunning


                        onClicked: {

                            dispatcher.stopData(
                                root.moduleId
                            )


                            root.updateRunningState()


                            statusText.text =
                                    "Live data stopped for "
                                    + root.moduleName


                            statusText.color =
                                    "#b9c1c9"


                            root.refreshGraph()
                        }
                    }
                }


                Text {

                    id: statusText


                    text:
                        "Select Excel and start data."


                    color: "#b9c1c9"


                    font.pixelSize: 13
                }
            }
        }


        // =================================================
        // TABLE + GRAPH
        // =================================================

        RowLayout {

            Layout.fillWidth: true

            Layout.fillHeight: true

            spacing: 12


            // =================================================
            // PARAMETER TABLE
            // =================================================

            Rectangle {

                Layout.fillHeight: true

                Layout.preferredWidth: 500

                Layout.minimumWidth: 380

                radius: 10

                color: "#20252b"


                ColumnLayout {

                    anchors.fill: parent

                    anchors.margins: 12

                    spacing: 5


                    Text {

                        text:
                            root.moduleName
                            + " PARAMETERS"


                        color: "white"


                        font.pixelSize: 16

                        font.bold: true
                    }


                    Rectangle {

                        Layout.fillWidth: true

                        Layout.preferredHeight: 40

                        radius: 5

                        color: "#15191e"


                        RowLayout {

                            anchors.fill: parent

                            anchors.margins: 8


                            Text {

                                text: "PARAMETER"

                                color: "white"

                                font.bold: true

                                Layout.preferredWidth: 190
                            }


                            Text {

                                text: "RAW"

                                color: "white"

                                font.bold: true

                                Layout.preferredWidth: 100
                            }


                            Text {

                                text: "VALUE"

                                color: "white"

                                font.bold: true

                                Layout.fillWidth: true
                            }
                        }
                    }


                    ListView {

                        id: parameterList


                        Layout.fillWidth: true

                        Layout.fillHeight: true


                        clip: true


                        model:
                            root.currentParsedData


                        delegate: Rectangle {

                            width:
                                parameterList.width


                            height: 42


                            color:
                                index % 2 === 0
                                ? "#282e35"
                                : "#20252b"


                            RowLayout {

                                anchors.fill: parent

                                anchors.margins: 8


                                Text {

                                    text:
                                        modelData.name !== undefined
                                        ? modelData.name
                                        : ""


                                    color: "white"


                                    Layout.preferredWidth: 190


                                    elide:
                                        Text.ElideRight
                                }


                                Text {

                                    text:
                                        modelData.raw !== undefined
                                        ? root.formatValue(
                                              modelData.raw
                                          )
                                        : ""


                                    color: "#b9c1c9"


                                    Layout.preferredWidth: 100
                                }


                                Text {

                                    text:
                                        modelData.value !== undefined
                                        ? root.formatValue(
                                              modelData.value
                                          )
                                        : ""


                                    color: "#42d77d"


                                    font.bold: true


                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }


            // =================================================
            // GRAPH
            // =================================================

            Rectangle {

                Layout.fillWidth: true

                Layout.fillHeight: true

                Layout.minimumWidth: 400

                radius: 10

                color: "#20252b"


                ColumnLayout {

                    anchors.fill: parent

                    anchors.margins: 12

                    spacing: 8


                    // =========================================
                    // GRAPH HEADER
                    // =========================================

                    RowLayout {

                        Layout.fillWidth: true


                        Text {

                            text:
                                root.moduleName
                                + " LIVE GRAPH"


                            color: "white"


                            font.pixelSize: 16

                            font.bold: true
                        }


                        Item {
                            Layout.fillWidth: true
                        }


                        Text {

                            text: "Parameter:"


                            color: "#b9c1c9"
                        }


                        ComboBox {

                            id: parameterBox


                            Layout.preferredWidth: 200


                            model:
                                root.parameterNames


                            onCurrentTextChanged: {

                                if (currentText === "")
                                    return


                                if (root.selectedParameter ===
                                        currentText)
                                {
                                    return
                                }


                                root.selectedParameter =
                                        currentText


                                root.saveGraphParameter(
                                    currentText
                                )


                                /*
                                    History burada SİLİNMİYOR.

                                    Dispatcher seçilen parametrenin
                                    arka plan history'sini döndürür.
                                */

                                root.refreshGraph()
                            }
                        }
                    }


                    // =========================================
                    // CANVAS BACKGROUND
                    // =========================================

                    Rectangle {

                        Layout.fillWidth: true

                        Layout.fillHeight: true

                        radius: 8

                        color: "#15191e"


                        Canvas {

                            id: graphCanvas


                            anchors.fill: parent

                            anchors.margins: 10


                            onWidthChanged: {
                                requestPaint()
                            }


                            onHeightChanged: {
                                requestPaint()
                            }


                            onPaint: {

                                var ctx =
                                        getContext("2d")


                                ctx.clearRect(
                                    0,
                                    0,
                                    width,
                                    height
                                )


                                // =================================
                                // DIMENSIONS
                                // =================================

                                var leftMargin = 65
                                var rightMargin = 20
                                var topMargin = 35
                                var bottomMargin = 45


                                var graphWidth =
                                        width
                                        - leftMargin
                                        - rightMargin


                                var graphHeight =
                                        height
                                        - topMargin
                                        - bottomMargin


                                if (graphWidth <= 0 ||
                                    graphHeight <= 0)
                                {
                                    return
                                }


                                // =================================
                                // AXIS
                                // =================================

                                ctx.strokeStyle =
                                        "#6d7680"


                                ctx.lineWidth = 1


                                ctx.beginPath()


                                ctx.moveTo(
                                    leftMargin,
                                    topMargin
                                )


                                ctx.lineTo(
                                    leftMargin,
                                    topMargin
                                    + graphHeight
                                )


                                ctx.lineTo(
                                    leftMargin
                                    + graphWidth,
                                    topMargin
                                    + graphHeight
                                )


                                ctx.stroke()


                                // =================================
                                // TITLE
                                // =================================

                                ctx.fillStyle =
                                        "#ffffff"


                                ctx.font =
                                        "bold 14px sans-serif"


                                var title =
                                        root.selectedParameter !== ""
                                        ? root.selectedParameter
                                          + " Live Data"
                                        : "Live Data"


                                ctx.fillText(
                                    title,
                                    leftMargin,
                                    20
                                )


                                // =================================
                                // EMPTY GRAPH
                                // =================================

                                if (root.graphHistory.length === 0)
                                {
                                    ctx.fillStyle =
                                            "#8b949e"


                                    ctx.font =
                                            "14px sans-serif"


                                    ctx.fillText(
                                        "No graph data for "
                                        + root.moduleName,
                                        leftMargin + 20,
                                        topMargin + 40
                                    )


                                    return
                                }


                                // =================================
                                // MIN MAX
                                // =================================

                                var minimum =
                                        Number(
                                            root.graphHistory[0]
                                        )


                                var maximum =
                                        Number(
                                            root.graphHistory[0]
                                        )


                                for (var i = 1;
                                     i < root.graphHistory.length;
                                     i++)
                                {
                                    var current =
                                            Number(
                                                root.graphHistory[i]
                                            )


                                    if (current < minimum)
                                        minimum = current


                                    if (current > maximum)
                                        maximum = current
                                }


                                var difference =
                                        maximum - minimum


                                var padding =
                                        difference * 0.20


                                if (padding <= 0)
                                {
                                    padding =
                                            Math.abs(maximum)
                                            * 0.05


                                    if (padding < 1)
                                        padding = 1
                                }


                                minimum -= padding

                                maximum += padding


                                var range =
                                        maximum - minimum


                                if (range <= 0)
                                    range = 1


                                // =================================
                                // Y LABELS
                                // =================================

                                ctx.fillStyle =
                                        "#b9c1c9"


                                ctx.font =
                                        "12px sans-serif"


                                ctx.fillText(
                                    root.formatValue(
                                        maximum
                                    ),
                                    5,
                                    topMargin + 5
                                )


                                ctx.fillText(
                                    root.formatValue(
                                        minimum
                                    ),
                                    5,
                                    topMargin
                                    + graphHeight
                                )


                                // =================================
                                // GRID
                                // =================================

                                ctx.strokeStyle =
                                        "#30363d"


                                ctx.lineWidth = 1


                                for (var g = 1;
                                     g < 5;
                                     g++)
                                {
                                    var gridY =
                                            topMargin
                                            +
                                            (
                                                graphHeight
                                                * g / 5
                                            )


                                    ctx.beginPath()


                                    ctx.moveTo(
                                        leftMargin,
                                        gridY
                                    )


                                    ctx.lineTo(
                                        leftMargin
                                        + graphWidth,
                                        gridY
                                    )


                                    ctx.stroke()
                                }


                                // =================================
                                // GRAPH LINE
                                // =================================

                                ctx.strokeStyle =
                                        "#42d77d"


                                ctx.lineWidth = 2


                                ctx.beginPath()


                                var count =
                                        root.graphHistory.length


                                for (var p = 0;
                                     p < count;
                                     p++)
                                {
                                    /*
                                        Son 50 örnek için sabit
                                        X ölçeği kullanıyoruz.
                                    */

                                    var x =
                                            leftMargin
                                            +
                                            (
                                                p / 49
                                            )
                                            *
                                            graphWidth


                                    var value =
                                            Number(
                                                root.graphHistory[p]
                                            )


                                    var normalized =
                                            (
                                                value
                                                - minimum
                                            )
                                            /
                                            range


                                    var y =
                                            topMargin
                                            +
                                            graphHeight
                                            -
                                            (
                                                normalized
                                                *
                                                graphHeight
                                            )


                                    if (p === 0)
                                    {
                                        ctx.moveTo(
                                            x,
                                            y
                                        )
                                    }
                                    else
                                    {
                                        ctx.lineTo(
                                            x,
                                            y
                                        )
                                    }
                                }


                                ctx.stroke()


                                // =================================
                                // CURRENT VALUE
                                // =================================

                                var lastValue =
                                        root.graphHistory[
                                            root.graphHistory.length
                                            - 1
                                        ]


                                ctx.fillStyle =
                                        "#42d77d"


                                ctx.font =
                                        "bold 14px sans-serif"


                                ctx.fillText(
                                    "Current: "
                                    + root.formatValue(
                                          lastValue
                                      ),
                                    leftMargin,
                                    height - 10
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}