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

bool isCorrect(const qsizetype &argBegin, const qsizetype &argSize, const qsizetype &pSize)
{
    if (argBegin > pSize - 1)
    {
        qInfo() << "'begin' le" <<  pSize - 1;
        return false;
    }
    if (argSize > pSize)
    {
        qInfo() << "'size' le" <<  pSize;
        return false;
    }
    if (argBegin + argSize > pSize)
    {
        qInfo() << "'begin' + 'size' le" <<  pSize;
        return false;
    }
    return true;
}

void process(const ADCMEmulateQuery &query)
{
    const ChannelMap pre;
    Decoder decoder(query.input.toStdString(), pre);
    auto p{decoder.positionsOfCMAPHeaders()};
    QList<long> positions{QVector<long>(p.begin(), p.end())};
    if (!positions.empty())
    {
        auto begin{query.begin};
        auto size{query.size};
        auto delay{query.delay};
        auto c{isCorrect(begin, size, positions.size())};
        if (!c)
        {
            return;
        }
        QFile inputFile(query.input);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qInfo() << "Can't open input file " << query.input;
            return;
        }
        positions.push_back(inputFile.size());

        QFile outputFile(query.output);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qInfo() << "Can't open output file" << query.output;
            return;
        }

        QDataStream in(&inputFile);
        QDataStream out(&outputFile);

        struct SelectedPosition {
            long position;
            qsizetype size;
        };

        QList<SelectedPosition> selectedPositions;

        for (auto i{begin}; i < begin + size; ++i)
        {
            selectedPositions.push_back({positions[i], positions[i + 1] - positions[i]});
        }

        for (const auto& item : selectedPositions)
        {
            QByteArray ba(item.size, 0);
            in.device()->seek(item.position);
            auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
            if (rb == ba.size())
            {
                if (delay)
                {
                    QThread::msleep(delay);
                    outputFile.resize(0);
                    out.device()->seek(0);
                    qInfo() << QDateTime::currentDateTime();
                }
                out.writeRawData(ba.data(), static_cast<int>(ba.size()));
            }
        }

        inputFile.close();
        outputFile.close();
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
