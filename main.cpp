#include <QCoreApplication>

#include "mainclass.h"

#include <signal.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("adcm-emulate-program");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainClass mainClass(QCoreApplication::instance());

    struct sigaction sa;
    sa.sa_handler = mainClass.callSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags |= SA_RESTART;
    if(sigaction(SIGINT, &sa, nullptr))
    {
        return 1;
    }

    return a.exec();
}
