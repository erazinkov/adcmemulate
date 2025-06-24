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

#include "myprocess.h"
#include "controller.h"
#include "mainclass.h"

#include <QDebug>
#include <csignal>

bool isCorrect(const qsizetype &argBegin, const qsizetype &argSize, const qsizetype &argOverlap, const qsizetype &pSize)
{
    if (argBegin > pSize - 1)
    {
        std::cout << "Ensure 'begin' <= " <<  pSize - 1 << std::endl;
        return false;
    }
    if (argSize > pSize)
    {
        std::cout << "Ensure 'size' <= " <<  pSize << std::endl;
        return false;
    }
    if (argBegin + argSize > pSize)
    {
        std::cout << "Ensure 'begin' + 'size' <= " << pSize << std::endl;
        return false;
    }
    if (argOverlap - argSize > 0)
    {
        std::cout << "Ensure 'overlap' < 'size'" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("adcm-emulate-program");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainClass mainClass(QCoreApplication::instance());

    struct sigaction hup;
    hup.sa_handler = mainClass.callSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;
    if(sigaction(SIGINT, &hup, nullptr))
    {
        return 1;
    }

    return a.exec();
}
