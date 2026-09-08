#include "connector.h"

#include <QFileInfo>


// =====================================================
// CONSTRUCTOR
// =====================================================

Connector::Connector(
    const QString &moduleName,
    QObject *parent
    )
    : QObject(parent),
    m_moduleName(moduleName),
    m_connected(false),
    m_powerOn(false),
    m_killed(false),
    m_mode("DATA VIEW"),
    m_sampleCounter(0),
    m_firmwareFile(nullptr),
    m_firmwareFileSize(0),
    m_firmwareProcessedBytes(0),
    m_firmwareChunkSize(256),
    m_firmwareProgress(0),
    m_firmwareUploading(false)
{
    // =================================================
    // FIRMWARE TIMER
    // =================================================

    m_firmwareTimer =
        new QTimer(this);


    /*
        Her 150 ms'de bir firmware'in
        bir chunk'ı işlenecek.
    */

    m_firmwareTimer->setInterval(
        150
        );


    connect(
        m_firmwareTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            // =========================================
            // DOSYA AÇIK MI?
            // =========================================

            if (!m_firmwareFile ||
                !m_firmwareFile->isOpen())
            {
                m_firmwareTimer->stop();

                m_firmwareUploading =
                    false;


                emit firmwareUploadingChanged(
                    false
                    );


                emit firmwareFinished(
                    false,
                    "Firmware file is not open."
                    );


                return;
            }


            // =========================================
            // SONUNA GELDİK Mİ?
            // =========================================

            if (m_firmwareFile->atEnd())
            {
                m_firmwareProgress =
                    100;


                m_firmwareProcessedBytes =
                    m_firmwareFileSize;


                emit firmwareProgressChanged(
                    100
                    );


                m_firmwareTimer->stop();


                m_firmwareFile->close();


                m_firmwareUploading =
                    false;


                emit firmwareUploadingChanged(
                    false
                    );


                emit logGenerated(
                    m_moduleName
                    + " Firmware upload completed"
                    );


                emit firmwareFinished(
                    true,
                    "Firmware upload completed successfully."
                    );


                return;
            }


            // =========================================
            // BİR CHUNK OKU
            // =========================================

            QByteArray chunk =
                m_firmwareFile->read(
                    m_firmwareChunkSize
                    );


            if (chunk.isEmpty() &&
                !m_firmwareFile->atEnd())
            {
                m_firmwareTimer->stop();


                m_firmwareUploading =
                    false;


                emit firmwareUploadingChanged(
                    false
                    );


                emit firmwareFinished(
                    false,
                    "Firmware file read error."
                    );


                m_firmwareFile->close();


                return;
            }


            /*
                ŞU AN GERÇEK CAN YOK.

                Bu yüzden chunk'ı gerçekten
                ECU'ya göndermiyoruz.

                İleride burada:

                sendFirmwareChunk(chunk);

                gibi gerçek gönderim olacak.
            */


            // =========================================
            // İŞLENEN BYTE
            // =========================================

            m_firmwareProcessedBytes +=
                chunk.size();


            if (m_firmwareProcessedBytes >
                m_firmwareFileSize)
            {
                m_firmwareProcessedBytes =
                    m_firmwareFileSize;
            }


            // =========================================
            // PROGRESS HESAPLA
            // =========================================

            if (m_firmwareFileSize > 0)
            {
                m_firmwareProgress =
                    static_cast<int>(
                        (
                            static_cast<double>(
                                m_firmwareProcessedBytes
                                )
                            /
                            static_cast<double>(
                                m_firmwareFileSize
                                )
                            )
                        *
                        100.0
                        );
            }
            else
            {
                m_firmwareProgress =
                    0;
            }


            if (m_firmwareProgress > 100)
            {
                m_firmwareProgress =
                    100;
            }


            emit firmwareProgressChanged(
                m_firmwareProgress
                );


            // =========================================
            // DOSYA BİTTİYSE
            // =========================================

            if (m_firmwareProcessedBytes >=
                m_firmwareFileSize)
            {
                m_firmwareProgress =
                    100;


                emit firmwareProgressChanged(
                    100
                    );


                m_firmwareTimer->stop();


                m_firmwareFile->close();


                m_firmwareUploading =
                    false;


                emit firmwareUploadingChanged(
                    false
                    );


                emit logGenerated(
                    m_moduleName
                    + " Firmware upload completed"
                    );


                emit firmwareFinished(
                    true,
                    "Firmware upload completed successfully."
                    );
            }
        }
        );
}


// =====================================================
// CONNECT
// =====================================================

void Connector::connectModule()
{
    if (m_connected)
        return;


    m_connected =
        true;


    emit connectionChanged(
        true
        );


    emit logGenerated(
        m_moduleName
        + " Connected"
        );
}


// =====================================================
// DISCONNECT
// =====================================================

void Connector::disconnectModule()
{
    if (!m_connected)
        return;


    if (m_firmwareUploading)
    {
        cancelFirmwareUpload();
    }


    m_connected =
        false;


    m_powerOn =
        false;


    emit connectionChanged(
        false
        );


    emit logGenerated(
        m_moduleName
        + " Disconnected"
        );
}


// =====================================================
// CONNECTION
// =====================================================

bool Connector::isConnected() const
{
    return m_connected;
}


// =====================================================
// MODULE NAME
// =====================================================

QString Connector::moduleName() const
{
    return m_moduleName;
}


// =====================================================
// POWER ON
// =====================================================

void Connector::powerOn()
{
    if (!m_connected)
    {
        emit logGenerated(
            m_moduleName
            + " Power ON failed: ECU disconnected"
            );

        return;
    }


    if (m_killed)
    {
        emit logGenerated(
            m_moduleName
            + " Power ON blocked after KILL"
            );

        return;
    }


    m_powerOn =
        true;


    emit logGenerated(
        m_moduleName
        + " POWER ON"
        );
}


// =====================================================
// POWER OFF
// =====================================================

void Connector::powerOff()
{
    m_powerOn =
        false;


    m_killed =
        false;


    emit logGenerated(
        m_moduleName
        + " POWER OFF"
        );
}


// =====================================================
// KILL
// =====================================================

void Connector::kill()
{
    m_powerOn =
        false;


    m_killed =
        true;


    emit logGenerated(
        m_moduleName
        + " KILL"
        );
}


// =====================================================
// MODE
// =====================================================

void Connector::setMode(
    const QString &mode
    )
{
    m_mode =
        mode;


    emit logGenerated(
        m_moduleName
        + " Mode changed: "
        + m_mode
        );
}


// =====================================================
// TEST DATA
// =====================================================

void Connector::requestTestData()
{
    if (!m_connected)
        return;


    m_sampleCounter++;


    quint16 n1 =
        4800
        +
        static_cast<quint16>(
            (m_sampleCounter % 20) * 20
            );


    quint16 egt =
        90
        +
        static_cast<quint16>(
            m_sampleCounter % 15
            );


    quint16 fuelFlow =
        230
        +
        static_cast<quint16>(
            (m_sampleCounter % 10) * 3
            );


    quint8 bleedValve =
        static_cast<quint8>(
            m_sampleCounter % 8
            );


    quint16 vibration =
        1100
        +
        static_cast<quint16>(
            (m_sampleCounter % 20) * 10
            );


    quint32 majorVersion =
        0x00020001;


    QByteArray data(
        16,
        static_cast<char>(0x00)
        );


    // N1
    data[0] =
        static_cast<char>(
            n1 & 0xFF
            );

    data[1] =
        static_cast<char>(
            (n1 >> 8) & 0xFF
            );


    // EGT
    data[2] =
        static_cast<char>(
            egt & 0xFF
            );

    data[3] =
        static_cast<char>(
            (egt >> 8) & 0xFF
            );


    // FUEL FLOW
    data[4] =
        static_cast<char>(
            fuelFlow & 0xFF
            );

    data[5] =
        static_cast<char>(
            (fuelFlow >> 8) & 0xFF
            );


    // BLEED VALVE
    data[6] =
        static_cast<char>(
            bleedValve
            );


    // RESERVED
    data[7] =
        static_cast<char>(
            0x00
            );


    // VIBRATION
    data[8] =
        static_cast<char>(
            vibration & 0xFF
            );

    data[9] =
        static_cast<char>(
            (vibration >> 8) & 0xFF
            );


    // VERSION
    data[10] =
        static_cast<char>(
            majorVersion & 0xFF
            );

    data[11] =
        static_cast<char>(
            (majorVersion >> 8) & 0xFF
            );

    data[12] =
        static_cast<char>(
            (majorVersion >> 16) & 0xFF
            );

    data[13] =
        static_cast<char>(
            (majorVersion >> 24) & 0xFF
            );


    emit dataReceived(
        data
        );
}


// =====================================================
// START FIRMWARE UPLOAD
// =====================================================

void Connector::startFirmwareUpload(
    const QString &filePath
    )
{
    // =================================================
    // CONNECTION
    // =================================================

    if (!m_connected)
    {
        emit firmwareFinished(
            false,
            "ECU is not connected."
            );


        emit logGenerated(
            m_moduleName
            + " Firmware upload failed: ECU disconnected"
            );


        return;
    }


    // =================================================
    // ALREADY RUNNING
    // =================================================

    if (m_firmwareUploading)
    {
        emit logGenerated(
            m_moduleName
            + " Firmware upload already running"
            );


        return;
    }


    QFileInfo fileInfo(
        filePath
        );


    // =================================================
    // FILE EXISTS?
    // =================================================

    if (!fileInfo.exists() ||
        !fileInfo.isFile())
    {
        emit firmwareFinished(
            false,
            "Firmware file does not exist."
            );


        return;
    }


    // =================================================
    // EXTENSION
    // =================================================

    QString extension =
        fileInfo
            .suffix()
            .toLower();


    if (extension != "bin" &&
        extension != "hex")
    {
        emit firmwareFinished(
            false,
            "Only .bin and .hex firmware files are supported."
            );


        return;
    }


    // =================================================
    // OLD FILE CLEANUP
    // =================================================

    if (m_firmwareFile)
    {
        if (m_firmwareFile->isOpen())
        {
            m_firmwareFile->close();
        }


        delete m_firmwareFile;

        m_firmwareFile =
            nullptr;
    }


    // =================================================
    // OPEN FILE
    // =================================================

    m_firmwareFile =
        new QFile(
            filePath
            );


    if (!m_firmwareFile->open(
            QIODevice::ReadOnly
            ))
    {
        delete m_firmwareFile;

        m_firmwareFile =
            nullptr;


        emit firmwareFinished(
            false,
            "Firmware file could not be opened."
            );


        return;
    }


    // =================================================
    // SAVE FILE INFORMATION
    // =================================================

    m_firmwareFilePath =
        filePath;


    m_firmwareFileName =
        fileInfo.fileName();


    m_firmwareFileSize =
        fileInfo.size();


    m_firmwareProcessedBytes =
        0;


    m_firmwareProgress =
        0;


    // =================================================
    // EMPTY FILE CHECK
    // =================================================

    if (m_firmwareFileSize <= 0)
    {
        m_firmwareFile->close();


        emit firmwareFinished(
            false,
            "Firmware file is empty."
            );


        return;
    }


    // =================================================
    // START
    // =================================================

    m_firmwareUploading =
        true;


    emit firmwareProgressChanged(
        0
        );


    emit firmwareUploadingChanged(
        true
        );


    emit logGenerated(
        m_moduleName
        + " Firmware upload started: "
        + m_firmwareFileName
        + " | Size: "
        + QString::number(
            m_firmwareFileSize
            )
        + " bytes"
        );


    m_firmwareTimer->start();
}


// =====================================================
// CANCEL FIRMWARE
// =====================================================

void Connector::cancelFirmwareUpload()
{
    if (!m_firmwareUploading)
        return;


    m_firmwareTimer->stop();


    if (m_firmwareFile &&
        m_firmwareFile->isOpen())
    {
        m_firmwareFile->close();
    }


    m_firmwareUploading =
        false;


    emit firmwareUploadingChanged(
        false
        );


    emit logGenerated(
        m_moduleName
        + " Firmware upload cancelled at "
        + QString::number(
            m_firmwareProgress
            )
        + "%"
        );


    emit firmwareFinished(
        false,
        "Firmware upload cancelled."
        );
}


// =====================================================
// FIRMWARE GETTERS
// =====================================================

bool Connector::isFirmwareUploading() const
{
    return m_firmwareUploading;
}


int Connector::firmwareProgress() const
{
    return m_firmwareProgress;
}


qint64 Connector::firmwareFileSize() const
{
    return m_firmwareFileSize;
}


qint64 Connector::firmwareProcessedBytes() const
{
    return m_firmwareProcessedBytes;
}


QString Connector::firmwareFileName() const
{
    return m_firmwareFileName;
}


QString Connector::firmwareFilePath() const
{
    return m_firmwareFilePath;
}