#ifndef ADCMEMULATEPARSER_H
#define ADCMEMULATEPARSER_H

#include <QString>
#include <QCommandLineParser>

#include "adcmemulatequery.h"


class ADCMEmulateParser
{

public:
    ADCMEmulateParser(QCommandLineParser &, ADCMEmulateQuery &);

    bool parseResult();

private:
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
    QCommandLineParser &parser_;
    ADCMEmulateQuery &query_;

    CommandLineParseResult parseCommandLine();
};

#endif // ADCMEMULATEPARSER_H
