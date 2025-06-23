#include "adcmemulateparser.h"

ADCMEmulateParser::ADCMEmulateParser()
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
            std::fputs(qPrintable(m_parser.helpText()), stderr);
            break;
        }
        case Status::VersionRequested:
            m_parser.showVersion();
        case Status::HelpRequested:
            m_parser.showHelp();
    }

    return r;
}

ADCMEmulateParser::CommandLineParseResult ADCMEmulateParser::parseCommandLine()
{
    using Status = CommandLineParseResult::Status;

    m_parser.setApplicationDescription("Emulate and handle adcm data program.");
    m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption beginOption("b", "Begin spill index (default = 0).", "begin", "0");
    const QCommandLineOption endOption("e", "End spill index (unused).", "end");
    const QCommandLineOption sizeOption("s", "Size of chunk (>= 1, default = 1).", "size");
    const QCommandLineOption tailOption("t", "Save tail chunk.");
    const QCommandLineOption numberOption("n", "Number of chunks (unused, >= 1, default = 1).", "number", "1");
    const QCommandLineOption delayOption("d",
                                         "Turn on adcm emulation mode with delay in msecs (>= 1000). "
                                         "In emulation mode size of chunk is 1.",
                                         "delay"
                                         );
    const QCommandLineOption overlapOption("o", "Size of chunk overlap (>= 0, default = 0).", "overlap", "0");

    m_parser.addOption(beginOption);

    m_parser.addOption(sizeOption);
    m_parser.addOption(tailOption);
    m_parser.addOption(delayOption);
    m_parser.addOption(overlapOption);
    m_parser.addOption(numberOption);
    m_parser.addOption(endOption);
    m_parser.addPositionalArgument("input", "Input file name.");
    m_parser.addPositionalArgument("output", "Output file name.");
    const QCommandLineOption helpOption = m_parser.addHelpOption();
    const QCommandLineOption versionOption = m_parser.addVersionOption();

    if (!m_parser.parse(QCoreApplication::arguments()))
        return { Status::Error, m_parser.errorText() };

    if (m_parser.isSet(versionOption))
        return { Status::VersionRequested };

    if (m_parser.isSet(helpOption))
        return { Status::HelpRequested };

    if (m_parser.isSet(beginOption)) {
        bool ok;
        m_query.begin = m_parser.value(beginOption).toUInt(&ok);
        if (m_query.begin < 0 || !ok)
        {
            return { Status::Error, QString("Incorrect begin spill number: %1").arg(m_query.begin) };
        }
    }

    if (m_parser.isSet(endOption)) {
        bool ok;
        m_query.end = m_parser.value(endOption).toUInt(&ok);
        if (m_query.end < m_query.begin || !ok)
        {
            return { Status::Error, QString("Incorrect end spill number: %1").arg(m_query.end) };
        }
    }

    if (m_parser.isSet(sizeOption)) {
        bool ok;
        m_query.size = m_parser.value(sizeOption).toUInt(&ok);
        if (m_query.size < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect chunk size: %1").arg(m_query.size) };
        }
    }

    if (m_parser.isSet(tailOption)) {
        m_query.tail = true;
    }

    if (m_parser.isSet(numberOption)) {
        bool ok;
        m_query.number = m_parser.value(numberOption).toUInt(&ok);
        if (m_query.number < 1 || !ok)
        {
            return { Status::Error, QString("Incorrect number of chunks: %1").arg(m_query.number) };
        }
    }

    if (m_parser.isSet(delayOption)) {
        bool ok;
        m_query.delay = m_parser.value(delayOption).toUInt(&ok);
        if (m_query.delay < 1'000 || !ok)
        {
            return { Status::Error, QString("Incorrect delay: %1").arg(m_query.delay) };
        }
    }

    if (m_parser.isSet(overlapOption)) {
        bool ok;
        m_query.overlap = m_parser.value(overlapOption).toUInt(&ok);
        if (m_query.overlap < 0 || !ok)
        {
            return { Status::Error, QString("Incorrect overlap: %1").arg(m_query.overlap) };
        }
    }

    const QStringList positionalArguments = m_parser.positionalArguments();
    if (positionalArguments.isEmpty() || positionalArguments.size() < 2)
    {
        return { Status::Error, "Arguments 'input' and 'output' required." };
    }
    if (positionalArguments.size() > 2)
    {
        return { Status::Error, "Several 'input' or 'output' arguments specified." };
    }
    m_query.input = positionalArguments.first();
    m_query.output = positionalArguments.last();

    return { Status::Ok };
}
