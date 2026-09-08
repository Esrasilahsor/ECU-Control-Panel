#include "excelparser.h"
#include "xlsxdocument.h"

using namespace QXlsx;


// =====================================================
// CONSTRUCTOR
// =====================================================

ExcelParser::ExcelParser(QObject *parent)
    : QObject(parent)
{
}


// =====================================================
// EXCEL YÜKLE
// =====================================================

bool ExcelParser::loadExcel(
    const QString &filePath
    )
{
    m_groups.clear();
    m_lastError.clear();

    Document document(filePath);

    if (!document.load())
    {
        m_lastError =
            "Excel file could not be opened.";

        return false;
    }

    /*
        Excel sütunların:

        A = RAM_ADDR
        B = DATA_WIDTH
        C = BIT_OFFSET
        D = DATA_TYPE
        E = DATA_NAME
        F = CONVERSION_FORMULA
    */

    int row = 2;

    int currentRamAddress = -1;

    while (true)
    {
        QVariant nameCell =
            document.read(row, 5);

        if (!nameCell.isValid() ||
            nameCell.toString().trimmed().isEmpty())
        {
            break;
        }

        // -----------------------------------------
        // RAM ADDRESS
        // -----------------------------------------

        QVariant ramCell =
            document.read(row, 1);

        if (ramCell.isValid() &&
            !ramCell.toString().trimmed().isEmpty())
        {
            currentRamAddress =
                ramCell.toInt();
        }

        if (currentRamAddress < 0)
        {
            m_lastError =
                "Invalid RAM address at row "
                + QString::number(row);

            return false;
        }

        // -----------------------------------------
        // PART OLUŞTUR
        // -----------------------------------------

        ParameterPart part;

        part.ramAddress =
            currentRamAddress;

        part.dataWidth =
            document.read(row, 2)
                .toInt();

        part.bitOffset =
            document.read(row, 3)
                .toInt();

        part.dataType =
            document.read(row, 4)
                .toString()
                .trimmed();

        part.dataName =
            document.read(row, 5)
                .toString()
                .trimmed();

        part.conversionFormula =
            document.read(row, 6)
                .toString()
                .trimmed();

        if (part.dataWidth <= 0)
        {
            m_lastError =
                "Invalid DATA_WIDTH at row "
                + QString::number(row);

            return false;
        }

        // -----------------------------------------
        // AYNI PARAMETRE GRUBUNU BUL
        // -----------------------------------------

        bool groupFound = false;

        for (ParameterGroup &group : m_groups)
        {
            if (group.ramAddress == part.ramAddress &&
                group.dataName == part.dataName)
            {
                group.parts.append(part);

                groupFound = true;

                break;
            }
        }

        // -----------------------------------------
        // YOKSA YENİ GRUP
        // -----------------------------------------

        if (!groupFound)
        {
            ParameterGroup group;

            group.ramAddress =
                part.ramAddress;

            group.dataName =
                part.dataName;

            group.dataType =
                part.dataType;

            group.conversionFormula =
                part.conversionFormula;

            group.parts.append(part);

            m_groups.append(group);
        }

        row++;
    }

    if (m_groups.isEmpty())
    {
        m_lastError =
            "No parameter definitions found.";

        return false;
    }

    return true;
}


// =====================================================
// DATA PARSE
// =====================================================

QVariantList ExcelParser::parseData(
    const QByteArray &data
    )
{
    QVariantList results;

    for (const ParameterGroup &group : m_groups)
    {
        quint64 rawValue =
            buildRawValue(
                data,
                group
                );

        QVariantMap result;

        result["ramAddress"] =
            group.ramAddress;

        result["name"] =
            group.dataName;

        result["dataType"] =
            group.dataType;

        result["formula"] =
            group.conversionFormula;

        result["raw"] =
            QVariant::fromValue<qulonglong>(
                rawValue
                );

        // -----------------------------------------
        // ENUM
        // -----------------------------------------

        if (group.conversionFormula.compare(
                "Enum",
                Qt::CaseInsensitive
                ) == 0)
        {
            /*
                Excel dosyanda enum isimleri:

                0 = Closed
                1 = Open

                şeklinde ayrıca tanımlanmamış.

                Bu nedenle şimdilik birleşmiş
                sayısal enum değerini gösteriyoruz.
            */

            result["value"] =
                QString::number(rawValue);

            result["isEnum"] =
                true;
        }

        // -----------------------------------------
        // NORMAL PARAMETRE
        // -----------------------------------------

        else
        {
            double physicalValue =
                applyConversion(
                    rawValue,
                    group.conversionFormula
                    );

            result["value"] =
                physicalValue;

            result["isEnum"] =
                false;
        }

        results.append(result);
    }

    return results;
}


// =====================================================
// GROUP RAW VALUE
// =====================================================

quint64 ExcelParser::buildRawValue(
    const QByteArray &data,
    const ParameterGroup &group
    ) const
{
    if (group.parts.isEmpty())
    {
        return 0;
    }

    // =================================================
    // ENUM ÖZEL DURUMU
    // =================================================

    if (group.conversionFormula.compare(
            "Enum",
            Qt::CaseInsensitive
            ) == 0)
    {
        /*
            Excel:

            RAM 6
            BIT 0
            BIT 1
            BIT 2

            Bunları tek tek 1 bit okuyup:

            bit2 bit1 bit0

            şeklinde tek enum değeri yapıyoruz.

            Örnek:

            bit0 = 1
            bit1 = 0
            bit2 = 1

            sonuç:

            101 binary = 5 decimal
        */

        quint64 enumValue = 0;

        for (const ParameterPart &part : group.parts)
        {
            quint64 bit =
                extractBits(
                    data,
                    part.ramAddress,
                    part.bitOffset,
                    1
                    );

            if (bit != 0)
            {
                enumValue |=
                    (
                        static_cast<quint64>(1)
                        << part.bitOffset
                        );
            }
        }

        return enumValue;
    }


    // =================================================
    // TEK PARÇALI NORMAL PARAMETRE
    // =================================================

    if (group.parts.size() == 1)
    {
        const ParameterPart &part =
            group.parts.first();

        return extractBits(
            data,
            part.ramAddress,
            part.bitOffset,
            part.dataWidth
            );
    }


    // =================================================
    // ÇOK PARÇALI PARAMETRE
    // ÖRNEK: MAJOR VERSION
    // =================================================

    quint64 combinedValue = 0;

    for (const ParameterPart &part : group.parts)
    {
        quint64 partValue =
            extractBits(
                data,
                part.ramAddress,
                part.bitOffset,
                part.dataWidth
                );

        /*
            Excel:

            BIT_OFFSET = 0
            DATA_WIDTH = 16

            ve:

            BIT_OFFSET = 16
            DATA_WIDTH = 16

            şeklindeyse:

            alt 16 bit
            +
            üst 16 bit

            birleştirilir.
        */

        combinedValue |=
            (
                partValue
                << part.bitOffset
                );
    }

    return combinedValue;
}


// =====================================================
// BİT ÇIKAR
// =====================================================

quint64 ExcelParser::extractBits(
    const QByteArray &data,
    int ramAddress,
    int bitOffset,
    int bitLength
    ) const
{
    if (ramAddress < 0)
        return 0;

    if (bitOffset < 0)
        return 0;

    if (bitLength <= 0 ||
        bitLength > 64)
    {
        return 0;
    }

    /*
        Offset byte'a dönüştürülür.

        Örneğin:

        RAM = 10
        bitOffset = 16

        16 / 8 = 2 byte

        yani okumaya RAM 12'den başlanır.
    */

    int byteOffset =
        bitOffset / 8;

    int localBitOffset =
        bitOffset % 8;

    int startByte =
        ramAddress + byteOffset;

    int totalBits =
        localBitOffset + bitLength;

    int bytesNeeded =
        (totalBits + 7) / 8;

    quint64 combined = 0;

    for (int i = 0;
         i < bytesNeeded;
         i++)
    {
        int byteIndex =
            startByte + i;

        if (byteIndex < 0 ||
            byteIndex >= data.size())
        {
            break;
        }

        quint8 currentByte =
            static_cast<quint8>(
                data.at(byteIndex)
                );

        combined |=
            static_cast<quint64>(
                currentByte
                )
            << (8 * i);
    }

    combined >>= localBitOffset;

    if (bitLength == 64)
    {
        return combined;
    }

    quint64 mask =
        (
            static_cast<quint64>(1)
            << bitLength
            )
        - 1;

    return combined & mask;
}


// =====================================================
// CONVERSION
// =====================================================

double ExcelParser::applyConversion(
    quint64 rawValue,
    const QString &formula
    ) const
{
    QString clean =
        formula
            .trimmed()
            .toLower();

    // x
    if (clean.isEmpty() ||
        clean == "x")
    {
        return static_cast<double>(
            rawValue
            );
    }

    // x * değer
    if (clean.contains("*"))
    {
        QStringList parts =
            clean.split("*");

        if (parts.size() == 2)
        {
            bool ok = false;

            double multiplier =
                parts.at(1)
                    .trimmed()
                    .toDouble(&ok);

            if (ok)
            {
                return
                    static_cast<double>(
                        rawValue
                        )
                    * multiplier;
            }
        }
    }

    // x + değer
    if (clean.contains("+"))
    {
        QStringList parts =
            clean.split("+");

        if (parts.size() == 2)
        {
            bool ok = false;

            double offset =
                parts.at(1)
                    .trimmed()
                    .toDouble(&ok);

            if (ok)
            {
                return
                    static_cast<double>(
                        rawValue
                        )
                    + offset;
            }
        }
    }

    // x - değer
    if (clean.contains("-"))
    {
        QStringList parts =
            clean.split("-");

        if (parts.size() == 2)
        {
            bool ok = false;

            double offset =
                parts.at(1)
                    .trimmed()
                    .toDouble(&ok);

            if (ok)
            {
                return
                    static_cast<double>(
                        rawValue
                        )
                    - offset;
            }
        }
    }

    return static_cast<double>(
        rawValue
        );
}


// =====================================================
// ERROR
// =====================================================

QString ExcelParser::lastError() const
{
    return m_lastError;
}