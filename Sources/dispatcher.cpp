#include "dispatcher.h"

#include <QDateTime>
#include <QUrl>
#include <QVariantMap>


// =====================================================
// CONSTRUCTOR
// =====================================================

Dispatcher::Dispatcher(
    QObject *parent
    )
    : QObject(parent),
    m_maxGraphPoints(50)
{
    // =================================================
    // CONNECTORS
    // =================================================

    m_ecuA1 =
        new Connector("ECU A.1", this);

    m_ecuA2 =
        new Connector("ECU A.2", this);

    m_ecuB1 =
        new Connector("ECU B.1", this);

    m_ecuB2 =
        new Connector("ECU B.2", this);


    // =================================================
    // PARSERS
    // =================================================

    m_parserA1 =
        new ExcelParser(this);

    m_parserA2 =
        new ExcelParser(this);

    m_parserB1 =
        new ExcelParser(this);

    m_parserB2 =
        new ExcelParser(this);


    // =================================================
    // DATA TIMERS
    // =================================================

    m_timerA1 =
        new QTimer(this);

    m_timerA2 =
        new QTimer(this);

    m_timerB1 =
        new QTimer(this);

    m_timerB2 =
        new QTimer(this);


    m_timerA1->setInterval(500);
    m_timerA2->setInterval(500);
    m_timerB1->setInterval(500);
    m_timerB2->setInterval(500);


    // =================================================
    // DATA TIMER SIGNALS
    // =================================================

    connect(
        m_timerA1,
        &QTimer::timeout,
        this,
        [this]()
        {
            m_ecuA1->requestTestData();
        }
        );


    connect(
        m_timerA2,
        &QTimer::timeout,
        this,
        [this]()
        {
            m_ecuA2->requestTestData();
        }
        );


    connect(
        m_timerB1,
        &QTimer::timeout,
        this,
        [this]()
        {
            m_ecuB1->requestTestData();
        }
        );


    connect(
        m_timerB2,
        &QTimer::timeout,
        this,
        [this]()
        {
            m_ecuB2->requestTestData();
        }
        );


    // =================================================
    // COMMON CONNECTOR SIGNALS
    // =================================================

    Connector *connectors[] =
        {
            m_ecuA1,
            m_ecuA2,
            m_ecuB1,
            m_ecuB2
        };


    for (Connector *connector : connectors)
    {
        connect(
            connector,
            &Connector::connectionChanged,
            this,
            [this](bool)
            {
                emit connectionStatesChanged();
            }
            );


        connect(
            connector,
            &Connector::logGenerated,
            this,
            [this](const QString &message)
            {
                addLog(message);
            }
            );


        connect(
            connector,
            &Connector::firmwareProgressChanged,
            this,
            [this](int)
            {
                emit firmwareStateChanged();
            }
            );


        connect(
            connector,
            &Connector::firmwareUploadingChanged,
            this,
            [this](bool)
            {
                emit firmwareStateChanged();
            }
            );
    }


    // =================================================
    // DATA SIGNALS
    // =================================================

    connect(
        m_ecuA1,
        &Connector::dataReceived,
        this,
        [this](const QByteArray &data)
        {
            handleIncomingData(
                "A1",
                data
                );
        }
        );


    connect(
        m_ecuA2,
        &Connector::dataReceived,
        this,
        [this](const QByteArray &data)
        {
            handleIncomingData(
                "A2",
                data
                );
        }
        );


    connect(
        m_ecuB1,
        &Connector::dataReceived,
        this,
        [this](const QByteArray &data)
        {
            handleIncomingData(
                "B1",
                data
                );
        }
        );


    connect(
        m_ecuB2,
        &Connector::dataReceived,
        this,
        [this](const QByteArray &data)
        {
            handleIncomingData(
                "B2",
                data
                );
        }
        );


    // =================================================
    // FIRMWARE FINISH SIGNALS
    // =================================================

    connect(
        m_ecuA1,
        &Connector::firmwareFinished,
        this,
        [this](
            bool success,
            const QString &message
            )
        {
            emit firmwareMessage(
                "A1",
                success,
                message
                );

            emit firmwareStateChanged();
        }
        );


    connect(
        m_ecuA2,
        &Connector::firmwareFinished,
        this,
        [this](
            bool success,
            const QString &message
            )
        {
            emit firmwareMessage(
                "A2",
                success,
                message
                );

            emit firmwareStateChanged();
        }
        );


    connect(
        m_ecuB1,
        &Connector::firmwareFinished,
        this,
        [this](
            bool success,
            const QString &message
            )
        {
            emit firmwareMessage(
                "B1",
                success,
                message
                );

            emit firmwareStateChanged();
        }
        );


    connect(
        m_ecuB2,
        &Connector::firmwareFinished,
        this,
        [this](
            bool success,
            const QString &message
            )
        {
            emit firmwareMessage(
                "B2",
                success,
                message
                );

            emit firmwareStateChanged();
        }
        );


    addLog(
        "Application started"
        );
}


// =====================================================
// NORMALIZE MODULE ID
// =====================================================

QString Dispatcher::normalizeModuleId(
    const QString &moduleId
    ) const
{
    QString id =
        moduleId
            .trimmed()
            .toUpper();


    if (id == "A1" ||
        id == "A.1" ||
        id == "ECU A1" ||
        id == "ECU A.1")
    {
        return "A1";
    }


    if (id == "A2" ||
        id == "A.2" ||
        id == "ECU A2" ||
        id == "ECU A.2")
    {
        return "A2";
    }


    if (id == "B1" ||
        id == "B.1" ||
        id == "ECU B1" ||
        id == "ECU B.1")
    {
        return "B1";
    }


    if (id == "B2" ||
        id == "B.2" ||
        id == "ECU B2" ||
        id == "ECU B.2")
    {
        return "B2";
    }


    return "";
}


// =====================================================
// CONNECTOR
// =====================================================

Connector *Dispatcher::getConnector(
    const QString &moduleId
    )
{
    QString id =
        normalizeModuleId(moduleId);


    if (id == "A1")
        return m_ecuA1;

    if (id == "A2")
        return m_ecuA2;

    if (id == "B1")
        return m_ecuB1;

    if (id == "B2")
        return m_ecuB2;


    return nullptr;
}


// =====================================================
// CONST CONNECTOR
// =====================================================

const Connector *Dispatcher::getConnectorConst(
    const QString &moduleId
    ) const
{
    QString id =
        normalizeModuleId(moduleId);


    if (id == "A1")
        return m_ecuA1;

    if (id == "A2")
        return m_ecuA2;

    if (id == "B1")
        return m_ecuB1;

    if (id == "B2")
        return m_ecuB2;


    return nullptr;
}


// =====================================================
// PARSER
// =====================================================

ExcelParser *Dispatcher::getParser(
    const QString &moduleId
    )
{
    QString id =
        normalizeModuleId(moduleId);


    if (id == "A1")
        return m_parserA1;

    if (id == "A2")
        return m_parserA2;

    if (id == "B1")
        return m_parserB1;

    if (id == "B2")
        return m_parserB2;


    return nullptr;
}


// =====================================================
// TIMER
// =====================================================

QTimer *Dispatcher::getTimer(
    const QString &moduleId
    )
{
    QString id =
        normalizeModuleId(moduleId);


    if (id == "A1")
        return m_timerA1;

    if (id == "A2")
        return m_timerA2;

    if (id == "B1")
        return m_timerB1;

    if (id == "B2")
        return m_timerB2;


    return nullptr;
}


const QTimer *Dispatcher::getTimer(
    const QString &moduleId
    ) const
{
    QString id =
        normalizeModuleId(moduleId);


    if (id == "A1")
        return m_timerA1;

    if (id == "A2")
        return m_timerA2;

    if (id == "B1")
        return m_timerB1;

    if (id == "B2")
        return m_timerB2;


    return nullptr;
}


// =====================================================
// CONNECT
// =====================================================

void Dispatcher::connectModule(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (!connector)
        return;


    connector->connectModule();
}


// =====================================================
// DISCONNECT
// =====================================================

void Dispatcher::disconnectModule(
    const QString &moduleId
    )
{
    stopData(moduleId);


    Connector *connector =
        getConnector(moduleId);


    if (!connector)
        return;


    connector->disconnectModule();
}


// =====================================================
// POWER
// =====================================================

void Dispatcher::powerOn(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (connector)
        connector->powerOn();
}


void Dispatcher::powerOff(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (connector)
        connector->powerOff();
}


void Dispatcher::killModule(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (connector)
        connector->kill();
}


// =====================================================
// MODE
// =====================================================

void Dispatcher::changeMode(
    const QString &moduleId,
    const QString &mode
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (connector)
    {
        connector->setMode(
            mode
            );
    }
}


// =====================================================
// EXCEL
// =====================================================

bool Dispatcher::loadExcel(
    const QString &moduleId,
    const QString &filePath
    )
{
    ExcelParser *parser =
        getParser(moduleId);


    if (!parser)
        return false;


    QString cleanPath =
        filePath;


    if (cleanPath.startsWith("file:"))
    {
        cleanPath =
            QUrl(cleanPath)
                .toLocalFile();
    }


    bool success =
        parser->loadExcel(
            cleanPath
            );


    if (success)
    {
        addLog(
            "Excel loaded for "
            + normalizeModuleId(moduleId)
            );
    }
    else
    {
        addLog(
            "Excel load failed for "
            + normalizeModuleId(moduleId)
            + ": "
            + parser->lastError()
            );
    }


    return success;
}


// =====================================================
// REQUEST DATA
// =====================================================

void Dispatcher::requestTestData(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (connector)
    {
        connector->requestTestData();
    }
}


// =====================================================
// START DATA
// =====================================================

void Dispatcher::startData(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    QTimer *timer =
        getTimer(moduleId);


    if (!connector ||
        !timer)
    {
        return;
    }


    if (!connector->isConnected())
        return;


    if (timer->isActive())
        return;


    connector->requestTestData();

    timer->start();


    emit dataRunningChanged();


    addLog(
        "Background data started for "
        + normalizeModuleId(moduleId)
        );
}


// =====================================================
// STOP DATA
// =====================================================

void Dispatcher::stopData(
    const QString &moduleId
    )
{
    QTimer *timer =
        getTimer(moduleId);


    if (!timer)
        return;


    if (!timer->isActive())
        return;


    timer->stop();


    emit dataRunningChanged();


    addLog(
        "Background data stopped for "
        + normalizeModuleId(moduleId)
        );
}


// =====================================================
// IS DATA RUNNING
// =====================================================

bool Dispatcher::isDataRunning(
    const QString &moduleId
    ) const
{
    const QTimer *timer =
        getTimer(moduleId);


    if (!timer)
        return false;


    return timer->isActive();
}


// =====================================================
// INCOMING DATA
// =====================================================

void Dispatcher::handleIncomingData(
    const QString &moduleId,
    const QByteArray &data
    )
{
    QString id =
        normalizeModuleId(moduleId);


    ExcelParser *parser =
        getParser(id);


    if (!parser)
        return;


    QVariantList parsed =
        parser->parseData(
            data
            );


    if (id == "A1")
        m_dataA1 = parsed;

    else if (id == "A2")
        m_dataA2 = parsed;

    else if (id == "B1")
        m_dataB1 = parsed;

    else if (id == "B2")
        m_dataB2 = parsed;


    storeGraphValues(
        id,
        parsed
        );


    emit parsedDataChanged();
}


// =====================================================
// STORE GRAPH
// =====================================================

void Dispatcher::storeGraphValues(
    const QString &moduleId,
    const QVariantList &parsedData
    )
{
    QString id =
        normalizeModuleId(moduleId);


    if (id.isEmpty())
        return;


    for (const QVariant &variant : parsedData)
    {
        QVariantMap item =
            variant.toMap();


        QString name =
            item.value("name")
                .toString();


        bool ok = false;


        double value =
            item.value("value")
                .toDouble(&ok);


        if (!ok ||
            name.isEmpty())
        {
            continue;
        }


        QVector<double> &history =
            m_graphHistory[id][name];


        history.append(value);


        while (
            history.size()
            >
            m_maxGraphPoints
            )
        {
            history.removeFirst();
        }
    }
}


// =====================================================
// GRAPH DATA
// =====================================================

QVariantList Dispatcher::graphData(
    const QString &moduleId,
    const QString &parameterName
    ) const
{
    QVariantList result;


    QString id =
        normalizeModuleId(moduleId);


    if (!m_graphHistory.contains(id))
        return result;


    QHash<QString, QVector<double>> ecuHistory =
        m_graphHistory.value(id);


    if (!ecuHistory.contains(parameterName))
        return result;


    QVector<double> values =
        ecuHistory.value(
            parameterName
            );


    for (double value : values)
    {
        result.append(value);
    }


    return result;
}


// =====================================================
// CLEAR GRAPH
// =====================================================

void Dispatcher::clearGraphData(
    const QString &moduleId
    )
{
    QString id =
        normalizeModuleId(moduleId);


    if (id.isEmpty())
        return;


    m_graphHistory[id].clear();


    emit parsedDataChanged();
}


// =====================================================
// START FIRMWARE
// =====================================================

void Dispatcher::startFirmwareUpload(
    const QString &moduleId,
    const QString &filePath
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (!connector)
        return;


    QString cleanPath =
        filePath;


    if (cleanPath.startsWith("file:"))
    {
        cleanPath =
            QUrl(cleanPath)
                .toLocalFile();
    }


    connector->startFirmwareUpload(
        cleanPath
        );


    emit firmwareStateChanged();
}


// =====================================================
// CANCEL FIRMWARE
// =====================================================

void Dispatcher::cancelFirmwareUpload(
    const QString &moduleId
    )
{
    Connector *connector =
        getConnector(moduleId);


    if (!connector)
        return;


    connector->cancelFirmwareUpload();


    emit firmwareStateChanged();
}


// =====================================================
// FIRMWARE PROGRESS
// =====================================================

int Dispatcher::firmwareProgress(
    const QString &moduleId
    ) const
{
    const Connector *connector =
        getConnectorConst(moduleId);


    if (!connector)
        return 0;


    return connector->firmwareProgress();
}


// =====================================================
// FIRMWARE RUNNING
// =====================================================

bool Dispatcher::isFirmwareUploading(
    const QString &moduleId
    ) const
{
    const Connector *connector =
        getConnectorConst(moduleId);


    if (!connector)
        return false;


    return connector->isFirmwareUploading();
}


// =====================================================
// FIRMWARE PROPERTY GETTERS
// =====================================================

int Dispatcher::a1FirmwareProgress() const
{
    return m_ecuA1->firmwareProgress();
}


int Dispatcher::a2FirmwareProgress() const
{
    return m_ecuA2->firmwareProgress();
}


int Dispatcher::b1FirmwareProgress() const
{
    return m_ecuB1->firmwareProgress();
}


int Dispatcher::b2FirmwareProgress() const
{
    return m_ecuB2->firmwareProgress();
}


bool Dispatcher::a1FirmwareUploading() const
{
    return m_ecuA1->isFirmwareUploading();
}


bool Dispatcher::a2FirmwareUploading() const
{
    return m_ecuA2->isFirmwareUploading();
}


bool Dispatcher::b1FirmwareUploading() const
{
    return m_ecuB1->isFirmwareUploading();
}


bool Dispatcher::b2FirmwareUploading() const
{
    return m_ecuB2->isFirmwareUploading();
}


// =====================================================
// PARSED DATA
// =====================================================

QVariantList Dispatcher::a1ParsedData() const
{
    return m_dataA1;
}


QVariantList Dispatcher::a2ParsedData() const
{
    return m_dataA2;
}


QVariantList Dispatcher::b1ParsedData() const
{
    return m_dataB1;
}


QVariantList Dispatcher::b2ParsedData() const
{
    return m_dataB2;
}


// =====================================================
// CONNECTION STATES
// =====================================================

bool Dispatcher::a1Connected() const
{
    return m_ecuA1->isConnected();
}


bool Dispatcher::a2Connected() const
{
    return m_ecuA2->isConnected();
}


bool Dispatcher::b1Connected() const
{
    return m_ecuB1->isConnected();
}


bool Dispatcher::b2Connected() const
{
    return m_ecuB2->isConnected();
}


bool Dispatcher::allConnected() const
{
    return
        a1Connected()
        &&
        a2Connected()
        &&
        b1Connected()
        &&
        b2Connected();
}


// =====================================================
// LOG
// =====================================================

QString Dispatcher::log() const
{
    return m_log;
}


void Dispatcher::addLog(
    const QString &message
    )
{
    QString time =
        QDateTime::currentDateTime()
            .toString("HH:mm:ss");


    if (!m_log.isEmpty())
        m_log += "\n";


    m_log +=
        "["
        + time
        + "] "
        + message;


    emit logChanged();
}