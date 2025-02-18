#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QCommandLineParser>
#include <QFile>

#include "decoder.h"

void spinner()
{
    static int pos{0};
    const char cursor[4]{'|', '/', '-', '\\'};
    std::cout << "\r" << cursor[pos] << std::flush;
    pos = (pos + 1) % 4;
}

struct ADCMEmulatorQuery
{
    QString input;
    QString output;
    long long begin{0};
    long long end{0};
    long long number{1};
    long long size{1};
};

struct CommandLineParseResult
{
    enum class Status {
        Ok,
        Error,
        VersionRequested,
        HelpRequested
    };
    Status statusCode = Status::Ok;
    std::optional<QString> errorString{std::nullopt};
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, ADCMEmulatorQuery &query)
{
    using Status = CommandLineParseResult::Status;

    parser.setApplicationDescription("Emulate and handle adcm data program.");
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption beginOption("b", "Begin spill index (default = 1)", "begin");
    const QCommandLineOption endOption("e", "End spill index (unused)", "end");
    const QCommandLineOption sizeOption("s", "Size of chunk (number of spills, default = 1)", "s");
    const QCommandLineOption numberOption("n", "Number of chunks (default = 1)", "n");

    parser.addOption(beginOption);
    parser.addOption(endOption);
    parser.addOption(sizeOption);
    parser.addOption(numberOption);
    parser.addPositionalArgument("input", "Input file name.");
    parser.addPositionalArgument("output", "Output file name.");
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments()))
        return { Status::Error, parser.errorText() };

    if (parser.isSet(versionOption))
        return { Status::VersionRequested };

    if (parser.isSet(helpOption))
        return { Status::HelpRequested };

    if (parser.isSet(beginOption)) {
        bool ok;
        query.begin = parser.value(beginOption).toUInt(&ok);
        if (query.begin < 0 || !ok)
        {
            return { Status::Error, QString("Incorrect begin spill number: %1").arg(query.begin) };
        }
    }

    if (parser.isSet(endOption)) {
        bool ok;
        query.end = parser.value(endOption).toUInt(&ok);
        if (query.end < query.begin || !ok)
        {
            return { Status::Error, QString("Incorrect end spill number: %1").arg(query.end) };
        }
    }

    if (parser.isSet(sizeOption)) {
        bool ok;
        query.size = parser.value(sizeOption).toUInt(&ok);
        if (query.size < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect chunk size: %1").arg(query.size) };
        }
    }

    if (parser.isSet(numberOption)) {
        bool ok;
        query.number = parser.value(numberOption).toUInt(&ok);
        if (query.number < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect number of chunks: %1").arg(query.number) };
        }
    }

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.isEmpty() || positionalArguments.size() < 2)
    {
        return { Status::Error, "Arguments 'input' and 'output' required." };
    }
    if (positionalArguments.size() > 2)
    {
        return { Status::Error, "Several 'input' or 'output' arguments specified." };
    }
    query.input = positionalArguments.first();
    query.output = positionalArguments.last();

    return { Status::Ok };
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

void process(const ADCMEmulatorQuery &query)
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("adcm-emulate-program");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QTimer::singleShot(0, [] ()
    {
        QCommandLineParser parser;
        ADCMEmulatorQuery query;
        using Status = CommandLineParseResult::Status;
        CommandLineParseResult parseResult = parseCommandLine(parser, query);
        switch (parseResult.statusCode) {
            case Status::Ok:
            {
                auto start = std::chrono::steady_clock::now();
                process(query);
                auto stop = std::chrono::steady_clock::now();
                std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
                break;
            }
            case Status::Error:
            {
                std::fputs(qPrintable(parseResult.errorString.value_or("Unknown error occurred")),
                           stderr);
                std::fputs("\n\n", stderr);
                std::fputs(qPrintable(parser.helpText()), stderr);
                break;
            }
            case Status::VersionRequested:
                parser.showVersion();
            case Status::HelpRequested:
                parser.showHelp();
        }
        QCoreApplication::exit(0);
    });


    return a.exec();
}
