#include "adcmemulateparser.h"

ADCMEmulateParser::ADCMEmulateParser(QCommandLineParser &parser, ADCMEmulateQuery &query) : parser_(parser), query_(query)
{
}

bool ADCMEmulateParser::parseResult()
{
    using Status = CommandLineParseResult::Status;

    auto r{false};

    auto parseCommandLineResult{parseCommandLine()};
    switch (parseCommandLineResult.statusCode) {
        case Status::Ok:
        {
            r = true;
            break;
        }
        case Status::Error:
        {
            std::fputs(qPrintable(parseCommandLineResult.errorString.value_or("Unknown error occurred")),
                       stderr);
            std::fputs("\n\n", stderr);
            std::fputs(qPrintable(parser_.helpText()), stderr);
            break;
        }
        case Status::VersionRequested:
            parser_.showVersion();
        case Status::HelpRequested:
            parser_.showHelp();
    }

    return r;
}

ADCMEmulateParser::CommandLineParseResult ADCMEmulateParser::parseCommandLine()
{
    using Status = CommandLineParseResult::Status;

    parser_.setApplicationDescription("Emulate and handle adcm data program.");
    parser_.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption beginOption("b", "Begin spill index (default = 0).", "begin");
    const QCommandLineOption endOption("e", "End spill index (unused).", "end");
    const QCommandLineOption sizeOption("s", "Size of chunk (>= 1, default = 1).", "size");
    const QCommandLineOption tailOption("t", "Save tail chunk.");
    const QCommandLineOption numberOption("n", "Number of chunks (unused, >= 1, default = 1).", "number");
    const QCommandLineOption delayOption("d",
                                         "Turn on adcm emulation mode with delay in msecs (>= 1000). "
                                         "In emulation mode size of chunk is 1.",
                                         "delay"
                                         );
    const QCommandLineOption overlapOption("o", "Size of chunk overlap (>= 0, default = 0).", "overlap");

    parser_.addOption(beginOption);

    parser_.addOption(sizeOption);
    parser_.addOption(tailOption);
    parser_.addOption(delayOption);
    parser_.addOption(overlapOption);
    parser_.addOption(numberOption);
    parser_.addOption(endOption);
    parser_.addPositionalArgument("input", "Input file name.");
    parser_.addPositionalArgument("output", "Output file name.");
    const QCommandLineOption helpOption = parser_.addHelpOption();
    const QCommandLineOption versionOption = parser_.addVersionOption();

    if (!parser_.parse(QCoreApplication::arguments()))
        return { Status::Error, parser_.errorText() };

    if (parser_.isSet(versionOption))
        return { Status::VersionRequested };

    if (parser_.isSet(helpOption))
        return { Status::HelpRequested };

    if (parser_.isSet(beginOption)) {
        bool ok;
        query_.begin = parser_.value(beginOption).toUInt(&ok);
        if (query_.begin < 0 || !ok)
        {
            return { Status::Error, QString("Incorrect begin spill number: %1").arg(query_.begin) };
        }
    }

    if (parser_.isSet(endOption)) {
        bool ok;
        query_.end = parser_.value(endOption).toUInt(&ok);
        if (query_.end < query_.begin || !ok)
        {
            return { Status::Error, QString("Incorrect end spill number: %1").arg(query_.end) };
        }
    }

    if (parser_.isSet(sizeOption)) {
        bool ok;
        query_.size = parser_.value(sizeOption).toUInt(&ok);
        if (query_.size < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect chunk size: %1").arg(query_.size) };
        }
    }

    if (parser_.isSet(tailOption)) {
        query_.tail = true;
    }

    if (parser_.isSet(numberOption)) {
        bool ok;
        query_.number = parser_.value(numberOption).toUInt(&ok);
        if (query_.number < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect number of chunks: %1").arg(query_.number) };
        }
    }

    if (parser_.isSet(delayOption)) {
        bool ok;
        query_.delay = parser_.value(delayOption).toUInt(&ok);
        if (query_.delay < 1'000 || !ok)
        {
            return { Status::Error, QString("Incorrect delay: %1").arg(query_.delay) };
        }
    }

    if (parser_.isSet(overlapOption)) {
        bool ok;
        query_.overlap = parser_.value(overlapOption).toUInt(&ok);
        if (query_.overlap < 0 || !ok)
        {
            return { Status::Error, QString("Incorrect overlap: %1").arg(query_.overlap) };
        }
    }

    const QStringList positionalArguments = parser_.positionalArguments();
    if (positionalArguments.isEmpty() || positionalArguments.size() < 2)
    {
        return { Status::Error, "Arguments 'input' and 'output' required." };
    }
    if (positionalArguments.size() > 2)
    {
        return { Status::Error, "Several 'input' or 'output' arguments specified." };
    }
    query_.input = positionalArguments.first();
    query_.output = positionalArguments.last();

    return { Status::Ok };
}
