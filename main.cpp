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
    std::vector<std::pair<u_int8_t, u_int8_t> > mapBi
    {
        {GAMMA, 0},
        {GAMMA, 1},
        {GAMMA, 2},
        {GAMMA, 3},
        {GAMMA, 4},
        {GAMMA, 5},
        {GAMMA, 6},
        {GAMMA, 7},
        {GAMMA, 8},
        {GAMMA, 9},
        {GAMMA, 10},
        {GAMMA, 11},
        {GAMMA, 12},
        {GAMMA, 13},
        {UNKNOWN, 111},
        {ALPHA, 0},
        {ALPHA, 1},
        {ALPHA, 2},
        {ALPHA, 3},
        {ALPHA, 4},
        {ALPHA, 5},
        {ALPHA, 6},
        {ALPHA, 7},
        {ALPHA, 8},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
        {UNKNOWN, 111},
    };

    const ChannelMap pre;
    const QString fileNameInput{"/home/egor/shares/tmp/proba_t2_thin_2"};

    Decoder decoder(fileNameInput.toStdString(), pre);
    auto p{decoder.positionsOfCMAPHeaders()};

    QList<long> positions{QVector<long>(p.begin(), p.end())};

    if (!positions.empty())
    {
        auto begin{0};
        auto size{30};
        auto overlap{0};
        QFile inputFile(fileNameInput);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qInfo() << "Can't open input file " << fileNameInput;
            return;
        }
        positions.push_back(inputFile.size());
        auto end{positions.size()};

        QDataStream in(&inputFile);

        struct SelectedPosition {
            long position;
            qsizetype size;
        };

        while (begin < end - size - overlap)
        {
            QString fileNameOutput{fileNameInput};
            QString postFix = QString("_from_%1_to_%2").arg(begin).arg(begin + size - 1);
            fileNameOutput.append(postFix);
            QFile outputFile(fileNameOutput);
            if (!outputFile.open(QIODevice::WriteOnly)) {
                qInfo() << "Can't open output file" << fileNameOutput;
                return;
            }
            QDataStream out(&outputFile);
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
                    out.writeRawData(ba.data(), static_cast<int>(ba.size()));
                }
            }
            qInfo() << "File:" << fileNameOutput << "From:" << begin << "To:" << begin + size - 1;
            outputFile.close();
            begin += size;
            begin -= overlap;
        }



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
