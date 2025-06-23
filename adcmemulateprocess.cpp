#include "adcmemulateprocess.h"

#include <QFile>
#include <QDebug>

ADCMEmulateProcess::ADCMEmulateProcess(const ADCMEmulateQuery query,
                                       const QList<qint64> offsets,
                                       QObject *parent) : QObject(parent), m_query(query)
{
    m_offsets.append(offsets);
    QFile inputFile(m_query.input);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qFatal("Can't open input file");
    }
    m_offsets.push_back(inputFile.size());
    inputFile.close();
}

void ADCMEmulateProcess::handle() const
{

    auto begin{m_query.begin};
    auto end{m_offsets.size()};

    QFile inputFile(m_query.input);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qInfo() << "Can't open input file " << m_query.input;
        return;
    }
    QDataStream in(&inputFile);

    while (begin < end - m_query.size - m_query.overlap)
    {
        QString fileNameOutput{m_query.output};
        QString postFix{QString("_%1_%2").arg(begin).arg(begin + m_query.size  + ( m_query.size != 1 ? -1 : 0))};
        fileNameOutput.append(postFix);
        QFile outputFile(fileNameOutput);
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            qFatal("Can't open output file");
        }
        QDataStream out(&outputFile);
        QList<SelectedPosition> selectedPositions;
        for (auto i{begin}; i < begin + m_query.size; ++i)
        {
            selectedPositions.push_back({m_offsets.at(i), m_offsets.at(i + 1) - m_offsets.at(i)});
        }

        qint64 pb{0};
        for (const auto& item : selectedPositions)
        {
            QByteArray ba(item.size, 0);
            in.device()->seek(item.position);
            auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
            if (rb == ba.size())
            {
                pb += rb;
                out.writeRawData(ba.data(), static_cast<int>(ba.size()));
            }
            else
            {
                qInfo() << "Incorrect size of spill at " << item.position;
                return;
            }
        }
        outputFile.close();

        begin += m_query.size;
        begin -= m_query.overlap;
        if (pb)
        {
            qInfo() << fileNameOutput;
        }
    }
    if (m_query.tail && begin < (end - 1))
    {
        auto size{(end - 1) - begin};
        QString fileNameOutput{m_query.output};
        QString postFix{QString("_%1_%2").arg(begin).arg(begin + size  + ( size != 1 ? -1 : 0))};
        fileNameOutput.append(postFix);
        QFile outputFile(fileNameOutput);
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            qFatal("Can't open output file");
        }
        QDataStream out(&outputFile);
        QList<SelectedPosition> selectedPositions;
        for (auto i{begin}; i < begin + size; ++i)
        {
            selectedPositions.push_back({m_offsets.at(i), m_offsets.at(i + 1) - m_offsets.at(i)});
        }

        auto pb{0};
        for (const auto& item : selectedPositions)
        {
            QByteArray ba(item.size, 0);
            in.device()->seek(item.position);
            auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
            if (rb == ba.size())
            {
                pb += rb;
                out.writeRawData(ba.data(), static_cast<int>(ba.size()));
            }
            else
            {
                qInfo() << "Incorrect size of spill at" << item.position;
                return;
            }
        }
        outputFile.close();
        begin += size;
        begin -= m_query.overlap;
        if (pb)
        {
            qInfo() << fileNameOutput;
        }
    }
    inputFile.close();
}
