#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QByteArray>
#include <QTimer>
#include <QHash>
#include <QVector>

#include "connector.h"
#include "excelparser.h"

class Dispatcher : public QObject
{
    Q_OBJECT

    // =====================================================
    // CONNECTION
    // =====================================================

    Q_PROPERTY(
        bool a1Connected
            READ a1Connected
                NOTIFY connectionStatesChanged
        )

    Q_PROPERTY(
        bool a2Connected
            READ a2Connected
                NOTIFY connectionStatesChanged
        )

    Q_PROPERTY(
        bool b1Connected
            READ b1Connected
                NOTIFY connectionStatesChanged
        )

    Q_PROPERTY(
        bool b2Connected
            READ b2Connected
                NOTIFY connectionStatesChanged
        )

    Q_PROPERTY(
        bool allConnected
            READ allConnected
                NOTIFY connectionStatesChanged
        )


    // =====================================================
    // LOG
    // =====================================================

    Q_PROPERTY(
        QString log
            READ log
                NOTIFY logChanged
        )


    // =====================================================
    // PARSED DATA
    // =====================================================

    Q_PROPERTY(
        QVariantList a1ParsedData
            READ a1ParsedData
                NOTIFY parsedDataChanged
        )

    Q_PROPERTY(
        QVariantList a2ParsedData
            READ a2ParsedData
                NOTIFY parsedDataChanged
        )

    Q_PROPERTY(
        QVariantList b1ParsedData
            READ b1ParsedData
                NOTIFY parsedDataChanged
        )

    Q_PROPERTY(
        QVariantList b2ParsedData
            READ b2ParsedData
                NOTIFY parsedDataChanged
        )


    // =====================================================
    // FIRMWARE PROGRESS
    // =====================================================

    Q_PROPERTY(
        int a1FirmwareProgress
            READ a1FirmwareProgress
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        int a2FirmwareProgress
            READ a2FirmwareProgress
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        int b1FirmwareProgress
            READ b1FirmwareProgress
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        int b2FirmwareProgress
            READ b2FirmwareProgress
                NOTIFY firmwareStateChanged
        )


    // =====================================================
    // FIRMWARE RUNNING
    // =====================================================

    Q_PROPERTY(
        bool a1FirmwareUploading
            READ a1FirmwareUploading
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        bool a2FirmwareUploading
            READ a2FirmwareUploading
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        bool b1FirmwareUploading
            READ b1FirmwareUploading
                NOTIFY firmwareStateChanged
        )

    Q_PROPERTY(
        bool b2FirmwareUploading
            READ b2FirmwareUploading
                NOTIFY firmwareStateChanged
        )


public:

    explicit Dispatcher(
        QObject *parent = nullptr
        );


    // =====================================================
    // CONNECTION
    // =====================================================

    bool a1Connected() const;
    bool a2Connected() const;
    bool b1Connected() const;
    bool b2Connected() const;

    bool allConnected() const;


    // =====================================================
    // DATA
    // =====================================================

    QVariantList a1ParsedData() const;
    QVariantList a2ParsedData() const;
    QVariantList b1ParsedData() const;
    QVariantList b2ParsedData() const;


    // =====================================================
    // LOG
    // =====================================================

    QString log() const;


    // =====================================================
    // FIRMWARE GETTERS
    // =====================================================

    int a1FirmwareProgress() const;
    int a2FirmwareProgress() const;
    int b1FirmwareProgress() const;
    int b2FirmwareProgress() const;

    bool a1FirmwareUploading() const;
    bool a2FirmwareUploading() const;
    bool b1FirmwareUploading() const;
    bool b2FirmwareUploading() const;


    // =====================================================
    // ECU CONTROL
    // =====================================================

    Q_INVOKABLE void connectModule(
        const QString &moduleId
        );

    Q_INVOKABLE void disconnectModule(
        const QString &moduleId
        );

    Q_INVOKABLE void powerOn(
        const QString &moduleId
        );

    Q_INVOKABLE void powerOff(
        const QString &moduleId
        );

    Q_INVOKABLE void killModule(
        const QString &moduleId
        );

    Q_INVOKABLE void changeMode(
        const QString &moduleId,
        const QString &mode
        );


    // =====================================================
    // EXCEL
    // =====================================================

    Q_INVOKABLE bool loadExcel(
        const QString &moduleId,
        const QString &filePath
        );


    // =====================================================
    // DATA
    // =====================================================

    Q_INVOKABLE void requestTestData(
        const QString &moduleId
        );

    Q_INVOKABLE void startData(
        const QString &moduleId
        );

    Q_INVOKABLE void stopData(
        const QString &moduleId
        );

    Q_INVOKABLE bool isDataRunning(
        const QString &moduleId
        ) const;


    // =====================================================
    // GRAPH
    // =====================================================

    Q_INVOKABLE QVariantList graphData(
        const QString &moduleId,
        const QString &parameterName
        ) const;

    Q_INVOKABLE void clearGraphData(
        const QString &moduleId
        );


    // =====================================================
    // FIRMWARE
    // =====================================================

    Q_INVOKABLE void startFirmwareUpload(
        const QString &moduleId,
        const QString &filePath
        );

    Q_INVOKABLE void cancelFirmwareUpload(
        const QString &moduleId
        );

    Q_INVOKABLE int firmwareProgress(
        const QString &moduleId
        ) const;

    Q_INVOKABLE bool isFirmwareUploading(
        const QString &moduleId
        ) const;


private:

    QString normalizeModuleId(
        const QString &moduleId
        ) const;


    Connector *getConnector(
        const QString &moduleId
        );


    const Connector *getConnectorConst(
        const QString &moduleId
        ) const;


    ExcelParser *getParser(
        const QString &moduleId
        );


    QTimer *getTimer(
        const QString &moduleId
        );


    const QTimer *getTimer(
        const QString &moduleId
        ) const;


    void handleIncomingData(
        const QString &moduleId,
        const QByteArray &data
        );


    void storeGraphValues(
        const QString &moduleId,
        const QVariantList &parsedData
        );


    void addLog(
        const QString &message
        );


private:

    Connector *m_ecuA1;
    Connector *m_ecuA2;
    Connector *m_ecuB1;
    Connector *m_ecuB2;


    ExcelParser *m_parserA1;
    ExcelParser *m_parserA2;
    ExcelParser *m_parserB1;
    ExcelParser *m_parserB2;


    QVariantList m_dataA1;
    QVariantList m_dataA2;
    QVariantList m_dataB1;
    QVariantList m_dataB2;


    QTimer *m_timerA1;
    QTimer *m_timerA2;
    QTimer *m_timerB1;
    QTimer *m_timerB2;


    QHash<
        QString,
        QHash<QString, QVector<double>>
        > m_graphHistory;


    int m_maxGraphPoints;

    QString m_log;


signals:

    void connectionStatesChanged();

    void logChanged();

    void parsedDataChanged();

    void dataRunningChanged();

    void firmwareStateChanged();

    void firmwareMessage(
        const QString &moduleId,
        bool success,
        const QString &message
        );
};

#endif // DISPATCHER_H