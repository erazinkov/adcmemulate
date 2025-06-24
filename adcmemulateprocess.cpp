#include "adcmemulateprocess.h"

#include <QFile>
#include <QDebug>
#include <QThread>
#include <QDateTime>

#include "spinner.h"

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

void ADCMEmulateProcess::process()
{
    m_query.delay ? emulate() : handle();
}

void ADCMEmulateProcess::handleProcess(long long &begin, const long long &size, QDataStream &in) const
{
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

    qint64 pb{0};
    for (const auto& item : selectedPositions)
    {
        QByteArray ba(item.size, 0);
        in.device()->seek(item.position);
        auto rb{in.readRawData(ba.data(), static_cast<int>(ba.size()))};
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

    begin += size;
    begin -= m_query.overlap;
    if (pb)
    {
        qInfo() << fileNameOutput;
    }
}

void ADCMEmulateProcess::handle() const
{
    auto begin{m_query.begin};
    auto end{m_offsets.size()};

    QFile inputFile(m_query.input);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qFatal("Can't open input file");
    }
    QDataStream in(&inputFile);

    while (begin < end - m_query.size - m_query.overlap && !this->thread()->isInterruptionRequested())
    {
        handleProcess(begin, m_query.size, in);
    }
    if (m_query.tail && begin < (end - 1) && !this->thread()->isInterruptionRequested())
    {
        auto size{(end - 1) - begin};
        handleProcess(begin, size, in);
    }
    inputFile.close();
}

void ADCMEmulateProcess::emulate() const
{
    auto begin{m_query.begin};
    auto end{m_offsets.size()};

    QFile inputFile(m_query.input);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qFatal("Can't open input file");
    }
    QDataStream in(&inputFile);

    QList<SelectedPosition> selectedPositions;
    for (auto i{begin}; i < end - 1; ++i)
    {
        selectedPositions.push_back({m_offsets.at(i), m_offsets.at(i + 1) - m_offsets.at(i)});
    }

    for (const auto& item : selectedPositions)
    {
        if (this->thread()->isInterruptionRequested())
        {
            break;
        }
        QByteArray ba(item.size, 0);
        in.device()->seek(item.position);
        auto rb{in.readRawData(ba.data(), static_cast<int>(ba.size()))};
        if (rb == ba.size())
        {
            QFile outputFile(m_query.output);
            if (!outputFile.open(QIODevice::WriteOnly))
            {
                qFatal("Can't open output file");
            }
            QDataStream out(&outputFile);
            auto wb{out.writeRawData(ba.data(), static_cast<int>(ba.size()))};
            if (rb != wb)
            {
                qInfo() << "Incorrect size of spill at" << item.position;
            }
            else
            {
                qInfo() << QString("%1").arg(wb) << QDateTime::currentDateTime().toString(Qt::TextDate);
            }
            outputFile.close();
            this->thread()->msleep(m_query.delay);
        }
        else
        {
            qInfo() << "Incorrect size of spill at" << item.position;
            return;
        }
    }
    inputFile.close();
}
