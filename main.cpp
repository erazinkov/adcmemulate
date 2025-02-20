#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QCommandLineParser>
#include <QFile>

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

    QList<long> qp{QVector<long>(p.begin(), p.end())};
    if (!qp.empty())
    {
        auto begin{query.begin};
        auto size{query.size};
        auto c{isCorrect(begin, size, qp.size())};
        if (!c)
        {
            return;
        }
        QFile inputFile(query.input);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qInfo() << "Can't open input file " << query.input;
            return;
        }
        qp.push_back(inputFile.size());

        QFile outputFile(query.output);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qInfo() << "Can't open output file" << query.output;
            return;
        }
        QDataStream in(&inputFile);
        in.device()->seek(qp[begin]);
        QDataStream out(&outputFile);
        for (auto i{begin}; i < begin + size; ++i)
        {
            QByteArray ba(qp[i + 1] - qp[i], 0);
            auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
            if (rb == ba.size())
            {
                out.writeRawData(ba.data(), static_cast<int>(ba.size()));
            }
            ba.clear();
        }
        inputFile.close();
        outputFile.close();
    }
}

void processBi()
{
    const ChannelMap pre;
    const QString fileNameInput{"/home/egor/shares/tmp/proba_t2_thin_2"};
    QString fileNameOutput{"test"};
    Decoder decoder(fileNameInput.toStdString(), pre);
    auto p{decoder.positionsOfCMAPHeaders()};

    QList<long> qp{QVector<long>(p.begin(), p.end())};
    if (!qp.empty())
    {
        QFile inputFile(fileNameInput);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qInfo() << "Can't open input file " << fileNameInput;
            return;
        }
        qp.push_back(inputFile.size());



//        QFile outputFile(query.output);
//        if (!outputFile.open(QIODevice::WriteOnly)) {
//            qInfo() << "Can't open output file" << query.output;
//            return;
//        }
//        QDataStream in(&inputFile);
//        in.device()->seek(qp[begin]);
//        QDataStream out(&outputFile);
//        for (auto i{begin}; i < begin + size; ++i)
//        {
//            QByteArray ba(qp[i + 1] - qp[i], 0);
//            auto rb = in.readRawData(ba.data(), static_cast<int>(ba.size()));
//            if (rb == ba.size())
//            {
//                out.writeRawData(ba.data(), static_cast<int>(ba.size()));
//            }
//            ba.clear();
//        }
        inputFile.close();
//        outputFile.close();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("adcm-emulate-program");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QTimer::singleShot(0, [] ()
    {
        processBi();
//        QCommandLineParser parser;
//        ADCMEmulateQuery query;
//        ADCMEmulateParser adcmEmulateParser(parser, query);
//        auto parseResult = adcmEmulateParser.parseResult();
//        if (parseResult)
//        {
//            auto start = std::chrono::steady_clock::now();
//            process(query);
//            auto stop = std::chrono::steady_clock::now();
//            std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
//        }
        QCoreApplication::exit(0);
    });


    return a.exec();
}
