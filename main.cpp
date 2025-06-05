#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QCommandLineParser>
#include <QFile>
#include <QThread>
#include <QDateTime>

#include "decoder.h"
#include "adcmemulatequery.h"
#include "adcmemulateparser.h"

void spinner()
{
    static int pos{0};
    const char cursor[4]{'|', '/', '-', '\\'};
    std::cout << "\r" << cursor[pos] << std::flush;
    pos = (pos + 1) % 4;
}

bool isCorrect(const qsizetype &argBegin, const qsizetype &argSize, const qsizetype &argOverlap, const qsizetype &pSize)
{
    if (argBegin > pSize - 1)
    {
        std::cout << "Ensure 'begin' -le " <<  pSize - 1 << std::endl;
        return false;
    }
    if (argSize > pSize)
    {
        std::cout << "Ensure 'size' -le " <<  pSize << std::endl;
        return false;
    }
    if (argBegin + argSize > pSize)
    {
        std::cout << "Ensure 'begin' + 'size' -le " << pSize << std::endl;
        return false;
    }
    if (argOverlap - argSize > 0)
    {
        std::cout << "Ensure 'overlap' -l 'size'" << std::endl;
        return false;
    }
    return true;
}

void process(const ADCMEmulateQuery &query)
{
    const ChannelMap pre = ChannelMap::mapNAP();
    Decoder decoder(query.input.toStdString(), pre);
    auto p{decoder.positionsOfCMAPHeaders()};
    std::cout << query.input.toStdString() << " - " << p.size() << std::endl;
    QList<long> positions{QVector<long>(p.begin(), p.end())};

    if (!positions.empty())
    {
        auto delay{query.delay};
        auto begin{query.begin};
        auto size{query.size};
        auto overlap{query.overlap};
        auto tail{query.tail};

        auto c{isCorrect(begin, size, overlap, positions.size())};
        if (!c)
        {
            return;
        }
        QFile inputFile(query.input);
        if (!inputFile.open(QIODevice::ReadOnly))
        {
            std::cout << "Can't open input file " << query.input.toStdString() << std::endl;
            return;
        }
        positions.push_back(inputFile.size());
        auto end{positions.size()};

        QDataStream in(&inputFile);

        struct SelectedPosition {
            long position;
            qsizetype size;
        };

        QList<SelectedPosition> selectedPositions;

        if (delay)
        {
            for (auto i{begin}; i < end - 1; ++i)
            {
                selectedPositions.push_back({positions[i], positions[i + 1] - positions[i]});
            }
            std::cout << std::left
                      << std::setw(20) << "Position"
                      << std::setw(20) << "Bytes"
                      << std::setw(20) << "Time"
                      << std::endl;
            for (const auto& item : selectedPositions)
            {
                QByteArray ba(item.size, 0);
                in.device()->seek(item.position);
                auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
                if (rb == ba.size())
                {
                    QFile outputFile(query.output);
                    if (!outputFile.open(QIODevice::WriteOnly))
                    {
                        std::cout << "Can't open output file " << query.output.toStdString() << std::endl;
                        return;
                    }
                    QDataStream out(&outputFile);
                    auto wb = out.writeRawData(ba.data(), static_cast<int>(ba.size()));
                    outputFile.close();
                    std::cout << std::left
                              << std::setw(20) << item.position
                              << std::setw(20) << wb                              << std::setw(20) << QDateTime::currentDateTime().toString().toStdString()
                              << std::endl;
                    QThread::msleep(delay);
                }
                else
                {
                    std::cout << "Incorrect size of spill at " << item.position << std::endl;
                    return;
                }
            }
            return;
        }


        std::cout << std::left
                  << std::setw(40) << "File"
                  << std::setw(30) << "Spills"
                  << std::setw(30) << "Bytes"
                  << std::setw(30) << "Time"
                  << std::endl;
        while (begin < end - size - overlap)
        {
            auto start = std::chrono::steady_clock::now();
            QString fileNameOutput{query.output};
            QString postFix = QString("_%1_%2").arg(begin).arg(begin + size  + ( size != 1 ? -1 : 0));
            fileNameOutput.append(postFix);
            QFile outputFile(fileNameOutput);
            if (!outputFile.open(QIODevice::WriteOnly))
            {
                std::cout << "Can't open output file" << fileNameOutput.toStdString() << std::endl;
                return;
            }
            QDataStream out(&outputFile);
            QList<SelectedPosition> selectedPositions;
            for (auto i{begin}; i < begin + size; ++i)
            {
                selectedPositions.push_back({positions[i], positions[i + 1] - positions[i]});
            }

            long long pb{0};
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
                    std::cout << "Incorrect size of spill at " << item.position << std::endl;
                    return;
                }
            }
            outputFile.close();

            begin += size;
            begin -= overlap;
            auto stop = std::chrono::steady_clock::now();
            if (pb)
            {
                std::cout << std::left
                          << std::setw(40) << fileNameOutput.toStdString()
                          << std::setw(30) << size
                          << std::setw(30) << pb
                          << std::setw(30) << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
                          << std::endl;
            }
        }
        if (tail && begin < (end - 1))
        {
            size = (end - 1) - begin;
            auto start = std::chrono::steady_clock::now();
            QString fileNameOutput{query.output};
            QString postFix = QString("_%1_%2").arg(begin).arg(begin + size  + ( size != 1 ? -1 : 0));
            fileNameOutput.append(postFix);
            QFile outputFile(fileNameOutput);
            if (!outputFile.open(QIODevice::WriteOnly))
            {
                std::cout << "Can't open output file" << fileNameOutput.toStdString() << std::endl;
                return;
            }
            QDataStream out(&outputFile);
            QList<SelectedPosition> selectedPositions;
            for (auto i{begin}; i < begin + size; ++i)
            {
                selectedPositions.push_back({positions[i], positions[i + 1] - positions[i]});
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
                    std::cout << "Incorrect size of spill at " << item.position << std::endl;
                    return;
                }
            }
            outputFile.close();
            begin += size;
            begin -= overlap;
            auto stop = std::chrono::steady_clock::now();
            if (pb)
            {
                std::cout << std::left
                          << std::setw(40) << fileNameOutput.toStdString()
                          << std::setw(30) << size
                          << std::setw(30) << pb
                          << std::setw(30) << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
                          << std::endl;
            }
        }
        inputFile.close();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("adcm-emulate-program");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QTimer::singleShot(0, [] ()
    {
        QCommandLineParser parser;
        ADCMEmulateQuery query;
        ADCMEmulateParser adcmEmulateParser(parser, query);
        auto parseResult = adcmEmulateParser.parseResult();
        if (parseResult)
        {
            auto start = std::chrono::steady_clock::now();
            process(query);
            auto stop = std::chrono::steady_clock::now();
            std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
        }
        QCoreApplication::exit(0);
    });


    return a.exec();
}
