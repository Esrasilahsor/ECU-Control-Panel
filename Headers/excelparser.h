#ifndef EXCELPARSER_H
#define EXCELPARSER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QVariantList>

class ExcelParser : public QObject
{
    Q_OBJECT

public:
    explicit ExcelParser(QObject *parent = nullptr);

    bool loadExcel(const QString &filePath);

    QVariantList parseData(const QByteArray &data);

    QString lastError() const;

private:

    struct ParameterPart
    {
        int ramAddress;
        int dataWidth;
        int bitOffset;

        QString dataType;
        QString dataName;
        QString conversionFormula;
    };

    struct ParameterGroup
    {
        int ramAddress;

        QString dataName;
        QString dataType;
        QString conversionFormula;

        QList<ParameterPart> parts;
    };

    QList<ParameterGroup> m_groups;

    QString m_lastError;

    quint64 extractBits(
        const QByteArray &data,
        int ramAddress,
        int bitOffset,
        int bitLength
        ) const;

    quint64 buildRawValue(
        const QByteArray &data,
        const ParameterGroup &group
        ) const;

    double applyConversion(
        quint64 rawValue,
        const QString &formula
        ) const;
};

#endif // EXCELPARSER_H