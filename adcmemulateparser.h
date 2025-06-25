#ifndef ADCMEMULATEPARSER_H
#define ADCMEMULATEPARSER_H

#include <QString>
#include <QCommandLineParser>

#include "adcmemulatequery.h"


class ADCMEmulateParser
{

public:
    ADCMEmulateParser();
    bool ok() const;

    const ADCMEmulateQuery &query() const;

private:
    bool m_ok{false};

    struct CommandLineParseResult {
        enum class Status {
            Ok,
            Error,
            VersionRequested,
            HelpRequested
        };
        Status statusCode{Status::Ok};
        std::optional<QString> errorString{std::nullopt};
    };
    QCommandLineParser m_parser;
    ADCMEmulateQuery m_query;

    CommandLineParseResult parseCommandLine();
};

#endif // ADCMEMULATEPARSER_H
