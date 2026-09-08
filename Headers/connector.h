#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QFile>

class Connector : public QObject
{
    Q_OBJECT

public:

    explicit Connector(
        const QString &moduleName,
        QObject *parent = nullptr
        );


    // =====================================================
    // CONNECTION
    // =====================================================

    void connectModule();
    void disconnectModule();

    bool isConnected() const;

    QString moduleName() const;


    // =====================================================
    // POWER
    // =====================================================

    void powerOn();
    void powerOff();
    void kill();


    // =====================================================
    // MODE
    // =====================================================

    void setMode(
        const QString &mode
        );


    // =====================================================
    // TEST DATA
    // =====================================================

    void requestTestData();


    // =====================================================
    // FIRMWARE
    // =====================================================

    void startFirmwareUpload(
        const QString &filePath
        );

    void cancelFirmwareUpload();

    bool isFirmwareUploading() const;

    int firmwareProgress() const;

    qint64 firmwareFileSize() const;

    qint64 firmwareProcessedBytes() const;

    QString firmwareFileName() const;

    QString firmwareFilePath() const;


signals:

    // =====================================================
    // CONNECTION
    // =====================================================

    void connectionChanged(
        bool connected
        );


    // =====================================================
    // LOG
    // =====================================================

    void logGenerated(
        const QString &message
        );


    // =====================================================
    // DATA
    // =====================================================

    void dataReceived(
        const QByteArray &data
        );


    // =====================================================
    // FIRMWARE
    // =====================================================

    void firmwareProgressChanged(
        int progress
        );

    void firmwareUploadingChanged(
        bool uploading
        );

    void firmwareFinished(
        bool success,
        const QString &message
        );


private:

    // =====================================================
    // GENERAL STATE
    // =====================================================

    QString m_moduleName;

    bool m_connected;

    bool m_powerOn;

    bool m_killed;

    QString m_mode;

    int m_sampleCounter;


    // =====================================================
    // FIRMWARE STATE
    // =====================================================

    QTimer *m_firmwareTimer;

    QFile *m_firmwareFile;

    QString m_firmwareFilePath;

    QString m_firmwareFileName;

    qint64 m_firmwareFileSize;

    qint64 m_firmwareProcessedBytes;

    qint64 m_firmwareChunkSize;

    int m_firmwareProgress;

    bool m_firmwareUploading;
};

#endif // CONNECTOR_H